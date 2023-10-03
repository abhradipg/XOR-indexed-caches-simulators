/**
 * lu.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 1024. */
#include "lu.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) (i+1)*(j+1)) / n;
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
      fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * n + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_lu(int n,
	       DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j, k;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 1998; c0 += 32)
    for (int c1 = c0; c1 <= 1999; c1 += 32)
      for (int c2 = c0; c2 <= 1999; c2 += 32) {
        for (int c3 = 0; c3 <= ppcg_min(ppcg_min(31, -c0 + c1 - 2), -c0 + c2 + 30); c3 += 1)
          for (int c4 = 0; c4 <= ppcg_min(31, -c1 + 1999); c4 += 1)
            for (int c5 = ppcg_max(0, c0 - c2 + c3 + 1); c5 <= ppcg_min(31, -c2 + 1999); c5 += 1)
              A[c1 + c4][c2 + c5] = (A[c1 + c4][c2 + c5] - (A[c1 + c4][c0 + c3] * A[c0 + c3][c2 + c5]));
        for (int c3 = ppcg_max(0, -c0 + c1 - 1); c3 <= ppcg_min(ppcg_min(ppcg_min(31, -c0 + 1998), -c0 + c1 + 30), -c0 + c2 + 30); c3 += 1) {
          for (int c5 = ppcg_max(0, c0 - c2 + c3 + 1); c5 <= ppcg_min(31, -c2 + 1999); c5 += 1) {
            A[c0 + c3][c2 + c5] = (A[c0 + c3][c2 + c5] / A[c0 + c3][c0 + c3]);
            A[c0 + c3 + 1][c2 + c5] = (A[c0 + c3 + 1][c2 + c5] - (A[c0 + c3 + 1][c0 + c3] * A[c0 + c3][c2 + c5]));
          }
          for (int c4 = c0 - c1 + c3 + 2; c4 <= ppcg_min(31, -c1 + 1999); c4 += 1)
            for (int c5 = ppcg_max(0, c0 - c2 + c3 + 1); c5 <= ppcg_min(31, -c2 + 1999); c5 += 1)
              A[c1 + c4][c2 + c5] = (A[c1 + c4][c2 + c5] - (A[c1 + c4][c0 + c3] * A[c0 + c3][c2 + c5]));
        }
      }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(A));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_lu (n, POLYBENCH_ARRAY(A));

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
