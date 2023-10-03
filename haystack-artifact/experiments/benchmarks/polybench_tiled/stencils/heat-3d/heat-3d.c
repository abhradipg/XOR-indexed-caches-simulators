/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* heat-3d.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "heat-3d.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_3D(A,N,N,N,n,n,n),
		 DATA_TYPE POLYBENCH_3D(B,N,N,N,n,n,n))
{
  int i, j, k;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
        A[i][j][k] = B[i][j][k] = (DATA_TYPE) (i + j + (n-k))* 10 / (n);
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_3D(A,N,N,N,n,n,n))

{
  int i, j, k;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("A");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++) {
         if ((i * n * n + j * n + k) % 20 == 0) fprintf(POLYBENCH_DUMP_TARGET, "\n");
         fprintf(POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, A[i][j][k]);
      }
  POLYBENCH_DUMP_END("A");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
__attribute__((noinline)) void kernel_heat_3d(int tsteps,
		      int n,
		      DATA_TYPE POLYBENCH_3D(A,N,N,N,n,n,n),
		      DATA_TYPE POLYBENCH_3D(B,N,N,N,n,n,n))
{
  int t, i, j, k;

#pragma scop
    /* ppcg generated CPU code */
    
    #define notused_min(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x < _y ? _x : _y; })
    #define notused_max(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x > _y ? _x : _y; })
    for (int c0 = 1; c0 <= 500; c0 += 1) {
      for (int c1 = 0; c1 <= 7; c1 += 1)
        for (int c2 = 0; c2 <= 7; c2 += 1)
          for (int c3 = 0; c3 <= 7; c3 += 1)
            for (int c4 = max(1, 16 * c1); c4 <= min(118, 16 * c1 + 15); c4 += 1)
              for (int c5 = max(1, 16 * c2); c5 <= min(118, 16 * c2 + 15); c5 += 1)
                for (int c6 = max(1, 16 * c3); c6 <= min(118, 16 * c3 + 15); c6 += 1)
                  B[c4][c5][c6] = ((((0.125 * ((A[c4 + 1][c5][c6] - (2. * 0.0)) + A[c4 - 1][c5][c6])) + (0.125 * ((A[c4][c5 + 1][c6] - (2. * 0.0)) + A[c4][c5 - 1][c6]))) + (0.125 * ((A[c4][c5][c6 + 1] - (2. * 0.0)) + A[c4][c5][c6 - 1]))) + A[c4][c5][c6]);
      for (int c1 = 0; c1 <= 7; c1 += 1)
        for (int c2 = 0; c2 <= 7; c2 += 1)
          for (int c3 = 0; c3 <= 7; c3 += 1)
            for (int c4 = max(1, 16 * c1); c4 <= min(118, 16 * c1 + 15); c4 += 1)
              for (int c5 = max(1, 16 * c2); c5 <= min(118, 16 * c2 + 15); c5 += 1)
                for (int c6 = max(1, 16 * c3); c6 <= min(118, 16 * c3 + 15); c6 += 1)
                  A[c4][c5][c6] = ((((0.125 * ((B[c4 + 1][c5][c6] - (2. * 0.0)) + B[c4 - 1][c5][c6])) + (0.125 * ((B[c4][c5 + 1][c6] - (2. * 0.0)) + B[c4][c5 - 1][c6]))) + (0.125 * ((B[c4][c5][c6 + 1] - (2. * 0.0)) + B[c4][c5][c6 - 1]))) + B[c4][c5][c6]);
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  POLYBENCH_3D_ARRAY_DECL(A, DATA_TYPE, N, N, N, n, n, n);
  POLYBENCH_3D_ARRAY_DECL(B, DATA_TYPE, N, N, N, n, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_heat_3d (tsteps, n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(A)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);

  return 0;
}
