/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* covariance.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "covariance.h"


/* Array initialization. */
static
void init_array (int m, int n,
		 DATA_TYPE *float_n,
		 DATA_TYPE POLYBENCH_2D(data,N,M,n,m))
{
  int i, j;

  *float_n = (DATA_TYPE)n;

  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
      data[i][j] = ((DATA_TYPE) i*j) / M;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int m,
		 DATA_TYPE POLYBENCH_2D(cov,M,M,m,m))

{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("cov");
  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++) {
      if ((i * m + j) % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
      fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, cov[i][j]);
    }
  POLYBENCH_DUMP_END("cov");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
__attribute__((noinline)) void kernel_covariance(int m, int n,
		       DATA_TYPE float_n,
		       DATA_TYPE POLYBENCH_2D(data,N,M,n,m),
		       DATA_TYPE POLYBENCH_2D(cov,M,M,m,m),
		       DATA_TYPE POLYBENCH_1D(mean,M,m))
{
  int i, j, k;

#pragma scop
  /* ppcg generated CPU code */
  
  #define notused_min(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x < _y ? _x : _y; })
  #define notused_max(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x > _y ? _x : _y; })
  {
    for (int c0 = 0; c0 <= 1199; c0 += 1)
      mean[c0] = 0.;
    for (int c0 = 0; c0 <= 74; c0 += 1)
      for (int c1 = 0; c1 <= 87; c1 += 1)
        for (int c2 = 16 * c0; c2 <= 16 * c0 + 15; c2 += 1)
          for (int c3 = 16 * c1; c3 <= min(1399, 16 * c1 + 15); c3 += 1)
            mean[c2] += data[c3][c2];
    for (int c0 = 0; c0 <= 1199; c0 += 1)
      mean[c0] /= float_n;
    for (int c0 = 0; c0 <= 74; c0 += 1)
      for (int c1 = c0; c1 <= 74; c1 += 1)
        for (int c2 = 16 * c0; c2 <= 16 * c0 + 15; c2 += 1)
          for (int c3 = max(16 * c1, c2); c3 <= 16 * c1 + 15; c3 += 1)
            cov[c2][c3] = 0.;
    for (int c0 = 0; c0 <= 87; c0 += 1)
      for (int c1 = 0; c1 <= 74; c1 += 1)
        for (int c2 = 16 * c0; c2 <= min(1399, 16 * c0 + 15); c2 += 1)
          for (int c3 = 16 * c1; c3 <= 16 * c1 + 15; c3 += 1)
            data[c2][c3] -= mean[c3];
    for (int c0 = 0; c0 <= 74; c0 += 1)
      for (int c1 = c0; c1 <= 74; c1 += 1)
        for (int c2 = 0; c2 <= 87; c2 += 1)
          for (int c3 = 16 * c0; c3 <= 16 * c0 + 15; c3 += 1)
            for (int c4 = max(16 * c1, c3); c4 <= 16 * c1 + 15; c4 += 1)
              for (int c5 = 16 * c2; c5 <= min(1399, 16 * c2 + 15); c5 += 1)
                cov[c3][c4] += (data[c5][c3] * data[c5][c4]);
    for (int c0 = 0; c0 <= 74; c0 += 1)
      for (int c1 = c0; c1 <= 74; c1 += 1)
        for (int c2 = 16 * c0; c2 <= 16 * c0 + 15; c2 += 1)
          for (int c3 = max(16 * c1, c2); c3 <= 16 * c1 + 15; c3 += 1) {
            cov[c2][c3] /= (float_n - 1.);
            cov[c3][c2] = cov[c2][c3];
          }
  }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int m = M;

  /* Variable declaration/allocation. */
  DATA_TYPE float_n;
  POLYBENCH_2D_ARRAY_DECL(data,DATA_TYPE,N,M,n,m);
  POLYBENCH_2D_ARRAY_DECL(cov,DATA_TYPE,M,M,m,m);
  POLYBENCH_1D_ARRAY_DECL(mean,DATA_TYPE,M,m);


  /* Initialize array(s). */
  init_array (m, n, &float_n, POLYBENCH_ARRAY(data));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_covariance (m, n, float_n,
		     POLYBENCH_ARRAY(data),
		     POLYBENCH_ARRAY(cov),
		     POLYBENCH_ARRAY(mean));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(m, POLYBENCH_ARRAY(cov)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(data);
  POLYBENCH_FREE_ARRAY(cov);
  POLYBENCH_FREE_ARRAY(mean);

  return 0;
}
