#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gmt.h>

#include "libget.h"

/*
 * This program reads a binary grid of dimension nx by ny and runs
 * a rectangular 2-D filter (dimension fx by fy) over it, assigning
 * the average value of all the points in the box to the output grid
 * point at the center of the filter. The program works in scanline
 * order (i.e., left to right, top to bottom). At the edges of the
 * grid the filter rolls in and rolls out (i.e., the first point
 * in the grid has filter points at the center point and (fx / 2) 
 * points to the right and (fy / 2) below, the second point has one 
 * point to the left and (fx / 2) points to the right and (fy / 2)
 * below, rolling along until the filter reached full width; it
 * rolls out in the opposite fashion as it reaches the right edge;
 * it then starts again at the left, adding a new row to the bottom,
 * and does the next scanline, repeating the process until the filter
 * reaches full height; when it reaches the bottom edge it rolls out
 * in an analogous way to the x-coordinate roll out).
 *
 * The rectangular operator is a simple boxcar filter: it sums all of 
 * the points inside it and divides by the number of points. Obviously
 * it would be stupid to do that every time, so each time the operator
 * shifts to the right, we subtract off the column on the left and 
 * add the new one on the right; likewise when shifting down a row
 * we strip off each point of the top row from its respective column 
 * and add the newly-added bottom row.
 * It's possible for roundoff error to accumulate using this method
 * but it doesn't seem to be a problem.
 *
 */

int main(int ac, char **av) {

  /* Input file */
  char in_path[256];

  /* Output file */
  char out_path[256];

  /* Size of the filter -- must be odd numbers */
  size_t fx;
  size_t fy;

  /* Size of the input grid */
  size_t nx;
  size_t ny;

  float **rows, *col_sum, *out, row_sum;
  size_t i, j;
  size_t first_row, last_row, first_col, last_col;
  size_t n_rows, n_cols, first_row_ix, last_row_ix;
  void *API; 
  struct GMT_GRID *Gin, *Gout;
  struct stat sbuf;
  int err;

  setpar(ac, av);
  mstpar("infile", "s", in_path);
  mstpar("outfile", "s", out_path);
  mstpar("fx", "z", &fx);
  mstpar("fy", "z", &fy);
  endpar();

  if (fx % 2 == 0) {
    fx++;
    fprintf(stderr, "Filter width must be odd, resetting to %zd\n", fx);
  }
  if (fy % 2 == 0) {
    fy++;
    fprintf(stderr, "Filter height must be odd, resetting to %zd\n", fy);
  }

  if (stat(out_path, &sbuf) == 0) {
    unlink(out_path);
  }

  if ((API = GMT_Create_Session("smooth", 0, 0, NULL)) == NULL) {
    fprintf(stderr, "Couldn't initiate GMT session\n");
    exit(-1);
  }

  /* Initialize the input object and open the file */
  fprintf(stderr, "Reading %s...", in_path);
  if ((Gin = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
			  GMT_IS_FILE, GMT_IS_SURFACE,
		      GMT_CONTAINER_AND_DATA, NULL,
		      in_path, NULL)) == NULL) {
    fprintf(stderr, "Couldn't read %s\n", in_path);
    exit(-1);
  }
  fprintf(stderr, "Done.\n");
  /* 
   * The output file has the same dimensions as the input 
   * so write the header, prep the output object, then open
   * the output for writing.
   */
  if ((Gout = GMT_Create_Data(API, GMT_IS_GRID, GMT_IS_SURFACE,
				  GMT_CONTAINER_AND_DATA, NULL,
				  Gin->header->wesn, Gin->header->inc,
				  GMT_GRID_NODE_REG, 0, NULL)) == NULL) {
    fprintf(stderr, "Couldn't create %s\n", out_path);
    exit(-1);
  }

  /* Get the grid dimensions */
  nx = Gin->header->n_columns;
  ny = Gin->header->n_rows;

  if (nx <= fx || ny <= fy) {
    fprintf(stderr, "Grid dimensions %zd x %zd smaller than filter dimensions %zd x %zd\n", 
            nx, ny, fx, fy);
    exit(-1);
  }

  /* Allocate space for fy arrays of floats */
  if ((rows = (float **)malloc(ny * sizeof(float*))) == NULL) {
    fprintf(stderr, "No memory for rows\n");
    exit(-1);
  }
  /* 
   * col_sum will hold the sums of all nx columns, each of which is the
   * height of the current filter (nominally fy points, but less during
   * roll in and roll out); zap it because it will be summed into.
   */
  if ((col_sum = (float *)malloc(nx * sizeof(float))) == NULL) {
    fprintf(stderr, "No memory for col_sum\n");
    exit(-1);
  }
  memset((void *)col_sum, 0, nx * sizeof(float));

  /* 
   * Prep the system; we're rolling in in both x and y, so they
   * start at 1/2 the ultimate filter width and height
   */
  first_row = 0;
  last_row  = fy / 2; /* works without the +1 because arrays are 0 offset */

  /* Prime the pump with the first fy/2+1 rows; note the <= in the for loop */
  for (j = first_row; j <= last_row; j++) {
    rows[j] = Gin->data + j * nx;
    for (i = 0; i < nx; i++) {
      col_sum[i] += rows[j][i];
    }
  }
  n_rows = last_row - first_row + 1;

  /* Now step through the output grid (nx * ny) first by row... */
  for (j = 0; j < ny; j++) {
    first_col = 0;
    last_col  = fx / 2; /* Again, works because arrays are 0 offset */
    n_cols = last_col - first_col + 1;

    /* Prime the pump with the first fx/2 + 1 columns */
    row_sum = 0;
    for (i = first_col; i <= last_col; i++) {
       row_sum += col_sum[i];
    }

    /* Step through each column of the jth row of the output */
	out = Gout->data + j * nx;
    for (i = 0; i < nx; i++) {
      /* compute the average of the points in the filter */
      out[i] = row_sum / (n_rows * n_cols);
      /* 
       * Now move the filter one point to the right.
       */
      /*
       * don't start dropping colunms from the left until we're 
       * done rolling in 
       */
      if (last_col >= (fx - 1)) {	
        row_sum -= col_sum[first_col];  
        first_col++;
      }
      /*
       * Add columns to the right until we start rolling out
       */
      if (last_col < (nx - 1)) {
        last_col++;
        row_sum += col_sum[last_col];
      }
      n_cols = last_col - first_col + 1;
    }
    /*
     * Shift down one row
     */
    /*
     * Don't start dropping rows from the top until we're 
     * done rolling in.
     */
    if (last_row >= (fy - 1)) {	
      /*
       * We only made "rows" with fy elements (which is all we need to
       * keep on hand), so we just keep rotating through them with 
       * the % operator here with the first element and in the next 
       * block with the last.
       */
      first_row_ix = first_row % fy;
      for (i = 0; i < nx; i++) {
        col_sum[i] -= rows[first_row_ix][i];
      }
      first_row++;
    }
    /*
     * Add rows to the bottom until we start rolling out
     */
    if (last_row < (ny - 1)) {
      last_row++;
      last_row_ix = last_row % fy;
      rows[last_row_ix] = Gin->data + last_row * nx;
      for (i = 0; i < nx; i++) {
        col_sum[i] += rows[last_row_ix][i];
      }
    }
    n_rows = last_row - first_row + 1;
    if ((j+1) % 100 == 0) {
      fprintf(stderr, "Done with %ld of %ld rows\n", j+1, ny);
    }
  }

  fprintf(stderr, "Writing %s...", out_path);
  if (GMT_Write_Data(API, GMT_IS_GRID,
			  GMT_IS_FILE, GMT_IS_SURFACE,
		      GMT_CONTAINER_AND_DATA, NULL,
		      out_path, Gout) != 0) {
    fprintf(stderr, "Couldn't write %s\n", out_path);
    exit(-1);
  }
  fprintf(stderr, "Done.\n");

  GMT_End_IO(API, GMT_IN, 0);
  GMT_End_IO(API, GMT_OUT, 0);
  GMT_Destroy_Session(API);

  free(col_sum);
  free(rows);
  return 0;
}
