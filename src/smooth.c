#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

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
 * The program only allocates space for and reads (fy * nx) points,
 * so if fy is considerably smaller than ny (and it should be) then
 * the program will use only a fraction of the memory required by
 * the full grid.
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
  struct GMT_GRDFILE Gin, Gout;
  GMT_LONG err;
  struct stat sbuf;

  setpar(ac, av);
  mstpar("infile", "s", in_path);
  mstpar("outfile", "s", out_path);
  mstpar("fx", "d", &fx);
  mstpar("fy", "d", &fy);
  endpar();

  if (fx % 2 == 0) {
    fx++;
    fprintf(stderr, "Filter width must be odd, resetting to %zd\n", fx);
  }
  if (fy % 2 == 0) {
    fy++;
    fprintf(stderr, "Filter height must be odd, resetting to %zd\n", fy);
  }

  if (stat(out_path, &sbuf) != ENOENT) {
    unlink(out_path);
  }

  GMT_begin(ac,av);

  /* Initialize the input object and open the file */
  GMT_grd_init(&Gin.header, ac, av, FALSE);
  if (GMT_open_grd(in_path, &Gin, 'r')) {
    fprintf(stderr, "Couldn't open %s\n", in_path);
    exit(-1);
  }

  /* Get the grid dimensions */
  nx = Gin.header.nx;
  ny = Gin.header.ny;

  if (nx <= fx || ny <= fy) {
    fprintf(stderr, "Grid dimensions %zd x %zd smaller than filter dimensions %zd x %zd\n", 
            nx, ny, fx, fy);
    exit(-1);
  }

  /* 
   * The output file has the same dimensions as the input 
   * so write the header, prep the output object, then open
   * the output for writing.
   */
  memcpy(&Gout.header, &Gin.header, sizeof(struct GRD_HEADER));
  memcpy(&Gout.header.name, out_path, GMT_LONG_TEXT);

  if (GMT_write_grd_info(out_path, &Gout.header)) {
    fprintf(stderr, "Couldn't write %s header\n", out_path);
    exit(-1);
  }

  GMT_grd_init(&Gout.header, ac, av, FALSE);
  memcpy(&Gout, &Gin, sizeof(struct GMT_GRDFILE));

  if ((err = GMT_open_grd(out_path, &Gout, 'w')) != 0) {
    fprintf(stderr, "Couldn't open %s: errno %ld\n", out_path, err);
    exit(-1);
  }

  /* Allocate space for fy arrays of nx floats */
  if ((rows = (float **)malloc(ny * sizeof(float*))) == NULL) {
    fprintf(stderr, "No memory for rows\n");
    exit(-1);
  }
  for (i = 0; i < fy; i++) {
    if ((rows[i] = (float *)malloc(nx * sizeof(float))) == NULL) {
      fprintf(stderr, "No memory for rows %zd\n", i);
      exit(-1);
    }
  }
  /* allocate the output array */
  if ((out = (float *)malloc(nx * sizeof(float))) == NULL) {
    fprintf(stderr, "No memory for out\n");
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
    if (GMT_read_grd_row(&Gin, (GMT_LONG)j, rows[j])) {
      fprintf(stderr, "Error reading %s at row %zd\n", in_path, j);
      exit(-1);
    }
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
    /* write the row we just completed */
    if (GMT_write_grd_row(&Gout, (GMT_LONG)j, out)) {
      fprintf(stderr, "Error writing %s\n", out_path);
      exit(-1);
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
      if (GMT_read_grd_row(&Gin, (GMT_LONG)j, rows[last_row_ix])) {
        fprintf(stderr, "Error reading %s at row %zd\n", in_path, last_row);
        exit(-1);
      }
      for (i = 0; i < nx; i++) {
        col_sum[i] += rows[last_row_ix][i];
      }
    }
    n_rows = last_row - first_row + 1;
    if ((j+1) % 100 == 0) {
      fprintf(stderr, "Done with %ld of %ld rows\n", j+1, ny);
    }
  }

  GMT_close_grd(&Gin);
  GMT_close_grd(&Gout);
  free(out);
  free(col_sum);
  for (i = 0; i < fy; i++) {
    free(rows[i]);
  }
  return 0;
}
