/**
 * fdtd-2d.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 50x1000x1000. */
#include "fdtd-2d.h"


/* Array initialization. */
static
void init_array (int tmax,
		 int nx,
		 int ny,
		 DATA_TYPE POLYBENCH_2D(ex,NX,NY,nx,ny),
		 DATA_TYPE POLYBENCH_2D(ey,NX,NY,nx,ny),
		 DATA_TYPE POLYBENCH_2D(hz,NX,NY,nx,ny),
		 DATA_TYPE POLYBENCH_1D(_fict_,TMAX,tmax))
{
  int i, j;

  for (i = 0; i < tmax; i++)
    _fict_[i] = (DATA_TYPE) i;
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      {
	ex[i][j] = ((DATA_TYPE) i*(j+1)) / nx;
	ey[i][j] = ((DATA_TYPE) i*(j+2)) / ny;
	hz[i][j] = ((DATA_TYPE) i*(j+3)) / nx;
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int nx,
		 int ny,
		 DATA_TYPE POLYBENCH_2D(ex,NX,NY,nx,ny),
		 DATA_TYPE POLYBENCH_2D(ey,NX,NY,nx,ny),
		 DATA_TYPE POLYBENCH_2D(hz,NX,NY,nx,ny))
{
  int i, j;

  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, ex[i][j]);
      fprintf(stderr, DATA_PRINTF_MODIFIER, ey[i][j]);
      fprintf(stderr, DATA_PRINTF_MODIFIER, hz[i][j]);
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_fdtd_2d(int tmax,
		    int nx,
		    int ny,
		    DATA_TYPE POLYBENCH_2D(ex,NX,NY,nx,ny),
		    DATA_TYPE POLYBENCH_2D(ey,NX,NY,nx,ny),
		    DATA_TYPE POLYBENCH_2D(hz,NX,NY,nx,ny),
		    DATA_TYPE POLYBENCH_1D(_fict_,TMAX,tmax))
{
  int t, i, j;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 49; c0 += 32)
    for (int c1 = c0; c1 <= ppcg_min(2048, c0 + 2030); c1 += 32)
      for (int c2 = c0; c2 <= ppcg_min(2048, c0 + 2030); c2 += 32)
        for (int c3 = ppcg_max(ppcg_max(0, -c0 + c1 - 1999), -c0 + c2 - 1999); c3 <= ppcg_min(31, -c0 + 49); c3 += 1) {
          if (c1 >= c0 + 32) {
            for (int c4 = 0; c4 <= ppcg_min(c3, c0 - c1 + c3 + 1999); c4 += 1)
              for (int c5 = ppcg_max(0, c0 - c2 + c3); c5 <= ppcg_min(31, c0 - c2 + c3 + 1999); c5 += 1) {
                ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] = (ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - (0.5 * (hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5])));
                if (c2 + c5 >= c0 + c3 + 1) {
                  ex[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] = (ex[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - (0.5 * (hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5 - 1])));
                  hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1] = (hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1] - (0.7 * (((ex[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5] - ex[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1]) + ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5 - 1]) - ey[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1])));
                }
              }
          } else {
            for (int c5 = ppcg_max(0, c0 - c2 + c3); c5 <= ppcg_min(31, c0 - c2 + c3 + 1999); c5 += 1) {
              if (c2 + c5 >= c0 + c3 + 1)
                ex[0][-c0 + c2 - c3 + c5] = (ex[0][-c0 + c2 - c3 + c5] - (0.5 * (hz[0][-c0 + c2 - c3 + c5] - hz[0][-c0 + c2 - c3 + c5 - 1])));
              ey[0][-c0 + c2 - c3 + c5] = _fict_[c0 + c3];
            }
          }
          for (int c4 = c3 + 1; c4 <= ppcg_min(31, c0 - c1 + c3 + 1999); c4 += 1)
            for (int c5 = ppcg_max(0, c0 - c2 + c3); c5 <= ppcg_min(31, c0 - c2 + c3 + 1999); c5 += 1) {
              ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] = (ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - (0.5 * (hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5])));
              if (c2 + c5 >= c0 + c3 + 1) {
                ex[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] = (ex[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - (0.5 * (hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5] - hz[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5 - 1])));
                hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1] = (hz[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1] - (0.7 * (((ex[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5] - ex[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1]) + ey[-c0 + c1 - c3 + c4][-c0 + c2 - c3 + c5 - 1]) - ey[-c0 + c1 - c3 + c4 - 1][-c0 + c2 - c3 + c5 - 1])));
              }
            }
        }
#pragma endscop
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int tmax = TMAX;
  int nx = NX;
  int ny = NY;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(ex,DATA_TYPE,NX,NY,nx,ny);
  POLYBENCH_2D_ARRAY_DECL(ey,DATA_TYPE,NX,NY,nx,ny);
  POLYBENCH_2D_ARRAY_DECL(hz,DATA_TYPE,NX,NY,nx,ny);
  POLYBENCH_1D_ARRAY_DECL(_fict_,DATA_TYPE,TMAX,tmax);

  /* Initialize array(s). */
  init_array (tmax, nx, ny,
	      POLYBENCH_ARRAY(ex),
	      POLYBENCH_ARRAY(ey),
	      POLYBENCH_ARRAY(hz),
	      POLYBENCH_ARRAY(_fict_));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_fdtd_2d (tmax, nx, ny,
		  POLYBENCH_ARRAY(ex),
		  POLYBENCH_ARRAY(ey),
		  POLYBENCH_ARRAY(hz),
		  POLYBENCH_ARRAY(_fict_));


  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(nx, ny, POLYBENCH_ARRAY(ex),
				    POLYBENCH_ARRAY(ey),
				    POLYBENCH_ARRAY(hz)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(ex);
  POLYBENCH_FREE_ARRAY(ey);
  POLYBENCH_FREE_ARRAY(hz);
  POLYBENCH_FREE_ARRAY(_fict_);

  return 0;
}
