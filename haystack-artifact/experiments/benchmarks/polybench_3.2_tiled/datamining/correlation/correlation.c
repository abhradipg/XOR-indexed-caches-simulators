/**
 * correlation.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 4000. */
#include "correlation.h"


/* Array initialization. */
static
void init_array (int m,
		 int n,
		 DATA_TYPE *float_n,
		 DATA_TYPE POLYBENCH_2D(data,M,N,m,n))
{
  int i, j;

  *float_n = 1.2;

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      data[i][j] = ((DATA_TYPE) i*j) / M;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int m,
		 DATA_TYPE POLYBENCH_2D(symmat,M,M,m,m))

{
  int i, j;

  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, symmat[i][j]);
      if ((i * m + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_correlation(int m, int n,
			DATA_TYPE float_n,
			DATA_TYPE POLYBENCH_2D(data,M,N,m,n),
			DATA_TYPE POLYBENCH_2D(symmat,M,M,m,m),
			DATA_TYPE POLYBENCH_1D(mean,M,m),
			DATA_TYPE POLYBENCH_1D(stddev,M,m))
{
  int i, j, j1, j2;

  DATA_TYPE eps = 0.1f;

#define sqrt_of_array_cell(x,j) sqrt(x[j])

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  {
    for (int c0 = 0; c0 <= 1999; c0 += 32)
      for (int c1 = 0; c1 <= 5997; c1 += 32)
        for (int c2 = 0; c2 <= ppcg_min(31, -c0 + 1999); c2 += 1) {
          if (c1 == 0)
            mean[c0 + c2] = 0.0;
          for (int c3 = 0; c3 <= ppcg_min(31, -c1 + 1999); c3 += 1)
            mean[c0 + c2] += data[c1 + c3][c0 + c2];
          if (c1 == 1984) {
            stddev[c0 + c2] = 0.0;
            mean[c0 + c2] /= float_n;
          }
          for (int c3 = ppcg_max(0, -c1 + 1999); c3 <= ppcg_min(31, -c1 + 3998); c3 += 1)
            stddev[c0 + c2] += ((data[c1 + c3 - 1999][c0 + c2] - mean[c0 + c2]) * (data[c1 + c3 - 1999][c0 + c2] - mean[c0 + c2]));
          if (c1 == 3968) {
            stddev[c0 + c2] /= float_n;
            stddev[c0 + c2] = sqrt(stddev[c0 + c2]);
            stddev[c0 + c2] = ((stddev[c0 + c2] <= eps) ? 1.0 : stddev[c0 + c2]);
          }
          for (int c3 = ppcg_max(0, -c1 + 3998); c3 <= ppcg_min(31, -c1 + 5997); c3 += 1) {
            data[c1 + c3 - 3998][c0 + c2] -= mean[c0 + c2];
            data[c1 + c3 - 3998][c0 + c2] /= (sqrt(float_n) * stddev[c0 + c2]);
          }
        }
    for (int c0 = 0; c0 <= 1998; c0 += 32)
      for (int c1 = c0; c1 <= 1999; c1 += 32)
        for (int c2 = 0; c2 <= 1999; c2 += 32)
          for (int c3 = 0; c3 <= ppcg_min(ppcg_min(31, -c0 + 1998), -c0 + c1 + 30); c3 += 1)
            for (int c4 = ppcg_max(0, c0 - c1 + c3 + 1); c4 <= ppcg_min(31, -c1 + 1999); c4 += 1) {
              if (c2 == 0)
                symmat[c0 + c3][c1 + c4] = 0.0;
              for (int c5 = 0; c5 <= ppcg_min(31, -c2 + 1999); c5 += 1)
                symmat[c0 + c3][c1 + c4] += (data[c2 + c5][c0 + c3] * data[c2 + c5][c1 + c4]);
              if (c2 == 1984)
                symmat[c1 + c4][c0 + c3] = symmat[c0 + c3][c1 + c4];
            }
    symmat[1999][1999] = 1.0;
    for (int c0 = 0; c0 <= 1998; c0 += 1)
      symmat[c0][c0] = 1.0;
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
  POLYBENCH_2D_ARRAY_DECL(data,DATA_TYPE,M,N,m,n);
  POLYBENCH_2D_ARRAY_DECL(symmat,DATA_TYPE,M,M,m,m);
  POLYBENCH_1D_ARRAY_DECL(mean,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(stddev,DATA_TYPE,M,m);

  /* Initialize array(s). */
  init_array (m, n, &float_n, POLYBENCH_ARRAY(data));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_correlation (m, n, float_n,
		      POLYBENCH_ARRAY(data),
		      POLYBENCH_ARRAY(symmat),
		      POLYBENCH_ARRAY(mean),
		      POLYBENCH_ARRAY(stddev));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(m, POLYBENCH_ARRAY(symmat)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(data);
  POLYBENCH_FREE_ARRAY(symmat);
  POLYBENCH_FREE_ARRAY(mean);
  POLYBENCH_FREE_ARRAY(stddev);

  return 0;
}
