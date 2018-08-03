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
 * gin is a base map into which we want to insert grid2 using
 * the weighted clipping mask gmask; all three input files must
 * be the same resolution and co-registered; grid2 and gmask must
 * be the same size and cover the exact same area, as well; the 
 * output file, gout, is the same size as gin
 */

const float defaultVs30 = 601.0;

char *mysprint(const char *fmt, int value);

int main(int ac, char **av) {

  /* Input files */
  char gin[256];
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

  void *API;
  struct GMT_GRID *G1, *G2, *Gmask, *Gout;
  struct GMT_GRID_HEADER *G_hdr;
  float *g1b, *g2b, *maskb, *outb;
  size_t i, j, nburn, npre;
  float val;
  int err;
  int grdcnt = 0;
  struct stat sbuf;

  setpar(ac, av);
  mstpar("gin", "s", gin);
  mstpar("gout", "s", gout);

  if (stat(gout, &sbuf) == 0) {
    unlink(gout);
  }

  API = GMT_Create_Session("insert_grd", 0, 0, NULL);

  /* Read the input grid */
  fprintf(stderr, "Reading input files...");
  if ((G1 = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
                  GMT_IS_FILE, GMT_IS_SURFACE,
                  GMT_CONTAINER_AND_DATA, NULL,
                  gin, NULL)) == NULL) {
    fprintf(stderr, "Couldn't read %s\n", gin);
    exit(-1);
  }

  /*
   * The output file has the same dimensions as gin
   * so write the header, prep the output object, then open
   * the output for writing.
   */
  if ((Gout = GMT_Create_Data(API, GMT_IS_GRID, GMT_IS_SURFACE,
                  GMT_CONTAINER_AND_DATA, NULL,
                  G1->header->wesn, G1->header->inc,
                  GMT_GRID_NODE_REG, 0, NULL)) == NULL) {
    fprintf(stderr, "Couldn't create %s\n", gout);
    exit(-1);
  }

  /* 
   * Just copy the input to the output -- we'll insert the
   * moasic tiles into the output grid
   */
  memcpy(Gout->data, G1->data, G1->header->size * sizeof(float));

  g1_x1 = G1->header->wesn[GMT_XLO];
  g1_x2 = G1->header->wesn[GMT_XHI];
  g1_y1 = G1->header->wesn[GMT_YLO];
  g1_y2 = G1->header->wesn[GMT_YHI];
  g1_nx = G1->header->n_columns;
  g1_ny = G1->header->n_rows;

  dx = G1->header->inc[0];
  dy = G1->header->inc[1];

  while(getpar(mysprint("grid%d", ++grdcnt), "s", grid2)) {
    mstpar(mysprint("gmask%d", grdcnt), "s", gmask);

    fprintf(stderr, "Inserting grid %s\n", grid2);

    if ((G2 = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
                  GMT_IS_FILE, GMT_IS_SURFACE,
                  GMT_CONTAINER_AND_DATA, NULL,
                  grid2, NULL)) == NULL) {
      fprintf(stderr, "Couldn't read %s\n", grid2);
      exit(-1);
    }
    if ((Gmask = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
                    GMT_IS_FILE, GMT_IS_SURFACE,
                    GMT_CONTAINER_AND_DATA, NULL,
                    gmask, NULL)) == NULL) {
      fprintf(stderr, "Couldn't read %s\n", gmask);
      exit(-1);
    }

    g2_x1 = G2->header->wesn[GMT_XLO];
    g2_x2 = G2->header->wesn[GMT_XHI];
    g2_y1 = G2->header->wesn[GMT_YLO];
    g2_y2 = G2->header->wesn[GMT_YHI];
    g2_nx = G2->header->n_columns;
    g2_ny = G2->header->n_rows;

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
     * burn off all the data in gin prior to the top row 
     * of grid2 (if any); the 0.1 is just to avoid roundoff
     * error 
     */
    nburn = (size_t)((g1_y2 - g2_y2) / dy + 0.1);

    /* 
     * Step through grid2: npre is the number of points in x before
     * we get to grid2
     */
    npre  = (size_t)((g2_x1 - g1_x1) / dx + 0.1);
    for (i = 0; i < g2_ny; ) {

      /* read, make weighted average, write */
      g1b = G1->data + (nburn + i) * g1_nx;
      outb = Gout->data + (nburn + i) * g1_nx;
      g2b = G2->data + i * g2_nx;
      maskb = Gmask->data + i * g2_nx;
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
        outb[j+npre] = val;
      }
      if ((++i) % 100 == 0) {
        fprintf(stderr, "Done with %zd rows of %zd\n", i, g2_ny);
      }
    }
    fprintf(stderr, "Done.\n");
  }
  endpar();

  fprintf(stderr, "Writing output...");
  if (GMT_Write_Data(API, GMT_IS_GRID,
              GMT_IS_FILE, GMT_IS_SURFACE,
              GMT_CONTAINER_AND_DATA, NULL,
              gout, Gout) != 0) {
    fprintf(stderr, "Couldn't write %s\n", gout);
    exit(-1);
  }
  fprintf(stderr, "Done.\n");

  GMT_End_IO(API, GMT_IN, 0);
  GMT_End_IO(API, GMT_OUT, 0);
  GMT_Destroy_Session(API);

  exit(0);
}

char *mysprint(const char *fmt, int value) {
  char *outstr = (char *)malloc(64 * sizeof(char));
  snprintf(outstr, 64 * sizeof(char), fmt, value);
  return outstr;
}
