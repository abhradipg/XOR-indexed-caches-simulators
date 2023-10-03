/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 20x1000. */
#include "seidel-2d.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*(j+2) + 2) / n;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n))

{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_seidel_2d(int tsteps,
		      int n,
		      DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int t, i, j;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c1 = 0; c1 <= 2017; c1 += 32)
    for (int c2 = c1; c2 <= ppcg_min(4034, c1 + 2048); c2 += 32)
      for (int c3 = ppcg_max(ppcg_max(ppcg_max(0, c1 - 1998), -c1 + c2 - 2029), (c2 / 2) - 1998); c3 <= ppcg_min(19, (c2 / 2) + 14); c3 += 1)
        for (int c4 = ppcg_max(ppcg_max(0, -c1 + c2 - c3 - 1998), -c1 + c3 + 1); c4 <= ppcg_min(ppcg_min(31, -c1 + c2 - c3 + 30), -c1 + c3 + 1998); c4 += 1)
          for (int c5 = ppcg_max(0, c1 - c2 + c3 + c4 + 1); c5 <= ppcg_min(31, c1 - c2 + c3 + c4 + 1998); c5 += 1)
            A[c1 - c3 + c4][-c1 + c2 - c3 - c4 + c5] = (((((((((A[c1 - c3 + c4 - 1][-c1 + c2 - c3 - c4 + c5 - 1] + A[c1 - c3 + c4 - 1][-c1 + c2 - c3 - c4 + c5]) + A[c1 - c3 + c4 - 1][-c1 + c2 - c3 - c4 + c5 + 1]) + A[c1 - c3 + c4][-c1 + c2 - c3 - c4 + c5 - 1]) + A[c1 - c3 + c4][-c1 + c2 - c3 - c4 + c5]) + A[c1 - c3 + c4][-c1 + c2 - c3 - c4 + c5 + 1]) + A[c1 - c3 + c4 + 1][-c1 + c2 - c3 - c4 + c5 - 1]) + A[c1 - c3 + c4 + 1][-c1 + c2 - c3 - c4 + c5]) + A[c1 - c3 + c4 + 1][-c1 + c2 - c3 - c4 + c5 + 1]) / 9.0);
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(A));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_seidel_2d (tsteps, n, POLYBENCH_ARRAY(A));

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
