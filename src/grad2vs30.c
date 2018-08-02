#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>

#include <gmt.h>

#include "libget.h"

/*
 * grad2vs30: convert topographic slope to Vs30
 *
 * Takes three input files, each formatted GMT grd files
 * and point for point co-registered with each other; 
 * gradient_file contains the topographic slope expressed as
 * a unitless ratio (e.g., meters per meter), landmask_file
 * is 0 for water and 1 (one) for land; craton_file is a
 * weight ranging from 1 (one) on stable shields (craton) and
 * 0 in active tectonic regions -- values in between will
 * be computed as the weighted average of the craton and 
 * tectonic models.
 * The optional numerical argument "water" is the value
 * that water-covered areas will be set to; the default is
 * 600.
 * The output file name is specified with "output_file"
 * (required).
 */

const float vs30_min = 180;
const float vs30_max = 900;

/* 
 * Slope to Vs30 uses Wald & Allen (2007) for craton, and
 * Allen & Wald (2009) for active tectonic.
 *
 * Split up the tables just in case future work makes them
 * have different numbers of rows
 * Columns are: vs30_min vs30_max slope_min slope_max
 */
const size_t rows_active = 6;
float active_table[6][4] = 
        {{180, 240, 3.0e-4,  3.5e-3},
         {240, 300, 3.5e-3,  0.01},
         {300, 360, 0.01,    0.018},
         {360, 490, 0.018,   0.05},
         {490, 620, 0.05,    0.10},
         {620, 760, 0.10,    0.14}};

const size_t rows_craton = 6;
float craton_table[6][4] = 
        {{180, 240, 2.0e-5,  2.0e-3},
         {240, 300, 2.0e-3,  4.0e-3},
         {300, 360, 4.0e-3,  7.2e-3},
         {360, 490, 7.2e-3,  0.013},
         {490, 620, 0.013,   0.018},
         {620, 760, 0.018,   0.025}};

/* 
 * Function interpVs30 interpolates (or extrapolates) vs30 from a
 * row of one of the tables (tables have already been log()'ed so
 * the exp() returns vs30 in linear units)
 * (I was going to pre-compute the differences (tt[1]-tt[0] and 
 * tt[3]-tt[2]) and make this function a #define for speed, but the
 * execution time is utterly dwarfed by the read/write times so there 
 * was no point.)
 */
float interpVs30(float *tt, float lg) {
  return exp(tt[0] + (tt[1] - tt[0]) * (lg - tt[2]) / (tt[3] - tt[2]));
}

int main(int ac, char **av) {

  /* Input files */
  char grad_path[256];
  char land_path[256];
  char craton_path[256];

  /* Output file */
  char vs30_path[256] = "global_vs30.grd";

  float water = 600;

  size_t nx, ny, m;
  float *grad, *land, *craton, *vs30;
  size_t nread, i, j, k, ndone = 0;
  size_t nr;
  float *tt, (*table)[4];
  float lg, vv, tvs[2];
  void *API = NULL;
  struct GMT_GRID *Ggrad, *Gcrat, *Gland, *Gout;
  struct GMT_GRID_HEADER *G_hdr;
  int err;
  struct stat sbuf;

  setlocale(LC_NUMERIC, "");

  setpar(ac, av);
  mstpar("gradient_file", "s", grad_path);
  mstpar("landmask_file", "s", land_path);
  mstpar("craton_file", "s", craton_path);
  mstpar("output_file", "s", vs30_path);
  getpar("water", "f", &water);
  endpar();

  if (stat(vs30_path, &sbuf) == 0) {
    unlink(vs30_path);
  }

  API = GMT_Create_Session("grad2vs30", 0, 0, NULL);

  /* Initialize the input objects and open the files */
  fprintf(stderr, "Reading input files...");
  if ((Ggrad = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
				  GMT_IS_FILE, GMT_IS_SURFACE,
				  GMT_CONTAINER_AND_DATA, NULL,
				  grad_path, NULL)) == NULL) {
    fprintf(stderr, "Couldn't read %s\n", grad_path);
    exit(-1);
  }
  if ((Gland = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
				  GMT_IS_FILE, GMT_IS_SURFACE,
				  GMT_CONTAINER_AND_DATA, NULL,
				  land_path, NULL)) == NULL) {
    fprintf(stderr, "Couldn't read %s\n", land_path);
    exit(-1);
  }
  if ((Gcrat = (struct GMT_GRID *)GMT_Read_Data(API, GMT_IS_GRID,
				  GMT_IS_FILE, GMT_IS_SURFACE,
				  GMT_CONTAINER_AND_DATA, NULL,
				  craton_path, NULL)) == NULL) {
    fprintf(stderr, "Couldn't read %s\n", craton_path);
    exit(-1);
  }
  fprintf(stderr, "Done.\n");

  G_hdr = Ggrad->header;
  nx = G_hdr->n_columns;
  ny = G_hdr->n_rows;

  /*
   * The output file has the same dimensions as Ggrad
   * so write the header, prep the output object, then open
   * the output for writing.
   */
  if ((Gout = GMT_Create_Data(API, GMT_IS_GRID, GMT_IS_SURFACE,
                  GMT_CONTAINER_AND_DATA, NULL,
                  Ggrad->header->wesn, Ggrad->header->inc,
                  GMT_GRID_NODE_REG, 0, NULL)) == NULL) {
    fprintf(stderr, "Couldn't create %s\n", vs30_path);
    exit(-1);
  }

  /* 
   * We're doing log-log interpolation, so log() everything in the 
   * tables first, for efficiency 
   */
  for (i = 0; i < rows_active; i++) {
    active_table[i][0] = log(active_table[i][0]);
    active_table[i][1] = log(active_table[i][1]);
    active_table[i][2] = log(active_table[i][2]);
    active_table[i][3] = log(active_table[i][3]);
  }
  for (i = 0; i < rows_craton; i++) {
    craton_table[i][0] = log(craton_table[i][0]);
    craton_table[i][1] = log(craton_table[i][1]);
    craton_table[i][2] = log(craton_table[i][2]);
    craton_table[i][3] = log(craton_table[i][3]);
  }

  for (m = 0; m < ny; m++) {
    grad = Ggrad->data + m * nx;
    land = Gland->data + m * nx;
    craton = Gcrat->data + m * nx;
	vs30 = Gout->data + m * nx;

    for (i = 0; i < nx; i++) {
      /* Set areas covered by water to the water value */
      if (land[i] == 0) {
        vs30[i] = water;
        continue;
      }

      /* This is the slope value to be converted to vs30 */
      lg = log(grad[i]);

      /* Get the Vs30 for both craton and active */
      for (k = 0; k < 2; k++) {

        /* k == 0 => craton, k == 1 => active */
        if (k == 0) {
          table = craton_table;
          nr = rows_craton;
        } else {
          table = active_table;
          nr = rows_active;
        }

        /* 
         * Handle slopes lower than the minimum in the table by
         * extrapolation capped by the minimum vs30 (this isn't
         * necessary when the table contains the minimum vs30,
         * but it's cheap insurance if we change the table or
         * minimum -- ditto for the max, below)
         */
        if (lg <= table[0][2]) {
          tt = table[0];
          vv = interpVs30(tt,lg);
          if (vv < vs30_min) {
            vv = vs30_min;
          }
          tvs[k] = vv;
          continue;
        }
        /* 
         * Handle slopes greater than the maximum in the table by 
         * extrapolation capped by the maximum vs30 
         */
        if (lg >= table[nr-1][3]) {
          tt = table[nr-1];
          vv = interpVs30(tt,lg);
          if (vv > vs30_max) {
            vv = vs30_max;
          }
          tvs[k] = vv;
          continue;
        }
        /* All other slopes should be handled within the tables */
        for (j = 0; j < nr; j++) {
          if (lg <= table[j][3]) {
            tt = table[j];
            tvs[k] = interpVs30(tt,lg);
            break;
          }
        }
      }
      /* Do a weighted average of craton and active vs30 */
      vs30[i] = craton[i] * tvs[0] + (1.0 - craton[i]) * tvs[1];
    }
    if(++ndone % 100 == 0) {
      fprintf(stderr,"Done with %'ld of %'ld elements\n", ndone * nx, ny * nx);
    }
  }

  fprintf(stderr, "Writing output file...");
  if (GMT_Write_Data(API, GMT_IS_GRID,
              GMT_IS_FILE, GMT_IS_SURFACE,
              GMT_CONTAINER_AND_DATA, NULL,
              vs30_path, Gout) != 0) {
    fprintf(stderr, "Couldn't write %s\n", vs30_path);
    exit(-1);
  }
  fprintf(stderr, "Done.\n");

  GMT_End_IO(API, GMT_IN, 0);
  GMT_End_IO(API, GMT_OUT, 0);
  GMT_Destroy_Session(API);

  return 0;
}
