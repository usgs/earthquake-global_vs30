#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include <gmt.h>

#include "libget.h"

/*
 * grid1 is a base map into which we want to insert grid2 using
 * the weighted clipping mask gmask; all three input files must
 * be the same resolution and co-registered; grid2 and gmask must
 * be the same size and cover the exact same area, as well; the 
 * output file, gout, is the same size as grid1
 */

const float defaultVs30 = 601.0;

int readNwrite(struct GMT_GRDFILE *in, struct GMT_GRDFILE *out, 
               float *buf, GMT_LONG row);

int main(int ac, char **av) {

  /* Input files */
  char grid1[256];
  char grid2[256];
  char gmask[256];

  /* Output file */
  char gout[256];

  /* Dimensions of the input grids */
  float g1_x1, g1_x2, g1_y1, g1_y2;
  size_t g1_nx, g1_ny;
  float g2_x1, g2_x2, g2_y1, g2_y2;
  size_t g2_nx, g2_ny;
  float dx, dy;

  struct GMT_GRDFILE G1, G2, Gmask, Gout;
  float *g1b, *g2b, *maskb;
  size_t i, j, nburn, npre;
  float val;
  GMT_LONG err;
  struct stat sbuf;

  setpar(ac, av);
  mstpar("grid1", "s", grid1);
  mstpar("grid2", "s", grid2);
  mstpar("gmask", "s", gmask);
  mstpar("gout", "s", gout);
  endpar();

  if (stat(gout, &sbuf) != ENOENT) {
    unlink(gout);
  }

  GMT_begin(ac,av);

  /* Initialize the input objects and open the files */
  GMT_grd_init(&G1.header, ac, av, FALSE);
  if (GMT_open_grd(grid1, &G1, 'r')) {
    fprintf(stderr, "Couldn't open %s\n", grid1);
    exit(-1);
  }

  GMT_grd_init(&G2.header, ac, av, FALSE);
  if (GMT_open_grd(grid2, &G2, 'r')) {
    fprintf(stderr, "Couldn't open %s\n", grid2);
    exit(-1);
  }

  GMT_grd_init(&Gmask.header, ac, av, FALSE);
  if (GMT_open_grd(gmask, &Gmask, 'r')) {
    fprintf(stderr, "Couldn't open %s\n", gmask);
    exit(-1);
  }

  g1_x1 = G1.header.x_min;
  g1_x2 = G1.header.x_max;
  g1_y1 = G1.header.y_min;
  g1_y2 = G1.header.y_max;
  g1_nx = G1.header.nx;
  g1_ny = G1.header.ny;

  g2_x1 = G2.header.x_min;
  g2_x2 = G2.header.x_max;
  g2_y1 = G2.header.y_min;
  g2_y2 = G2.header.y_max;
  g2_nx = G2.header.nx;
  g2_ny = G2.header.ny;

  dx = G1.header.x_inc;
  dy = G1.header.y_inc;

  /* Do some sanity checks */
  if (g1_x1 >= g1_x2 || g1_y1 >= g1_y2 || 
      g2_x1 >= g2_x2 || g2_y1 >= g2_y2) {
    fprintf(stderr, "Improper grid specification. x1,y1 should be < x2,y2\n");
    exit(-1);
  }
  if (g2_x1 < g1_x1 || g2_x2 > g1_x2 ||
      g2_y1 < g1_y1 || g2_y2 > g1_y2) { 
    fprintf(stderr, "Error: grid 2 must fit entirely within grid 1\n");
    exit(-1);
  }

  /*
   * The output file has the same dimensions as grid1
   * so write the header, prep the output object, then open
   * the output for writing.
   */
  memcpy(&Gout.header, &G1.header, sizeof(struct GRD_HEADER));
  memcpy(&Gout.header.name, gout, GMT_LONG_TEXT);

  if (GMT_write_grd_info(gout, &Gout.header)) {
    fprintf(stderr, "Couldn't write %s header\n", gout);
    exit(-1);
  }

  GMT_grd_init(&Gout.header, ac, av, FALSE);
  memcpy(&Gout, &G1, sizeof(struct GMT_GRDFILE));

  if ((err = GMT_open_grd(gout, &Gout, 'w')) != 0) {
    fprintf(stderr, "Couldn't open %s: errno %ld\n", gout, err);
    exit(-1);
  }

  /* 
   * Allocate a buffer for reading and writing a row of grid1/gout
   */
  if ((g1b = (float *)malloc(g1_nx * sizeof(float))) == NULL) {
    fprintf(stderr, "No memory for g1b\n");
    exit(-1);
  }
  /* 
   * Allocate working space
   */
  if ((g2b = (float *)malloc(g2_nx * sizeof(float))) == NULL) {
    fprintf(stderr, "No memory for g2b\n");
    exit(-1);
  }
  if ((maskb = (float *)malloc(g2_nx * sizeof(float))) == NULL) {
    fprintf(stderr, "No memory for maskb\n");
    exit(-1);
  }


  /* 
   * burn off all the data in grid1 prior to the top row 
   * of grid2 (if any); the 0.1 is just to avoid roundoff
   * error 
   */
  nburn = (size_t)((g1_y2 - g2_y2) / dy + 0.1);
  for (i = 0; i < nburn; ) {
    if (readNwrite(&G1, &Gout, g1b, i) < 0) {
      fprintf(stderr, "Error reading/writing row %zd (preroll)\n", i);
      exit(-1);
    }
    if (++i % 100 == 0) {
      fprintf(stderr, "Done with %zd rows of %zd\n", i, g1_ny);
    }
  }

  /* 
   * Step through grid2: npre is the number of points in x before
   * we get to grid2
   */
  npre  = (size_t)((g2_x1 - g1_x1) / dx + 0.1);
  for (i = 0; i < g2_ny; ) {

    /* read, make weighted average, write */
    if (GMT_read_grd_row(&G1,    i + nburn, g1b) ||
        GMT_read_grd_row(&G2,    i,         g2b) ||
        GMT_read_grd_row(&Gmask, i,         maskb)) {
      fprintf(stderr, "Error reading row %zd\n", i);
      exit(-1);
    }
    for (j = 0; j < g2_nx; j++) {
      val = g2b[j] * maskb[j] + g1b[j+npre] * (1 - maskb[j]);
      /* 
       * It's possible for the smoothed mask to be non-zero outside 
       * of the border (consider a region with a concave outer border
       * like California's eastern border), so here we check and 
       * fix up the output point.
       */
      if (g2b[j] == 0 && maskb[j] > 0) {
        if (g1b[j+npre] == 0) {
          fprintf(stderr,"Bad point x=%zd y=%zd, setting to %f\n",
                  i, j, defaultVs30);
          val = defaultVs30;
        } else {
          /* 
           * This is the "normal" situation; just use the background 
           * grid 
           */
          val = g1b[j+npre];
        }
      }
      g1b[j+npre] = val;
    }
    if (GMT_write_grd_row(&Gout, (GMT_LONG)i + nburn, g1b)) {
      fprintf(stderr, "Error writing row %zd\n", i);
      return(-1);
    }
    if ((++i + nburn) % 100 == 0) {
      fprintf(stderr, "Done with %zd rows of %zd\n", i + nburn, g1_ny);
    }
  }

  /*
   * Burn off the data below the insert grid, if any
   */
  for (i = nburn + g2_ny; i < g1_ny; ) {
    if (readNwrite(&G1, &Gout, g1b, i) < 0) {
      fprintf(stderr, "Error reading/writing row %zd (postroll)\n", i);
      exit(-1);
    }
    if (++i % 100 == 0) {
      fprintf(stderr, "Done with %zd rows of %zd\n", i, g1_ny);
    }
  }

  GMT_close_grd(&G1);
  GMT_close_grd(&G2);
  GMT_close_grd(&Gmask);
  GMT_close_grd(&Gout);
  free((void *)g1b);
  free((void *)g2b);
  free((void *)maskb);
  exit(0);
}

int readNwrite(struct GMT_GRDFILE *in, struct GMT_GRDFILE *out, float *buf, GMT_LONG row) {

  if (GMT_read_grd_row(in, row, buf)) {
    fprintf(stderr, "readNwrite: Error reading\n");
    return(-1);
  }
  if (GMT_write_grd_row(out, row, buf)) {
    fprintf(stderr, "readNwrite: Error writing\n");
    return(-1);
  }
  return 0;
}
