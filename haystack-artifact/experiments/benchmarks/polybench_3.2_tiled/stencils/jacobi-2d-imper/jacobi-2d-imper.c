/**
 * jacobi-2d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "jacobi-2d-imper.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
		 DATA_TYPE POLYBENCH_2D(B,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      {
	A[i][j] = ((DATA_TYPE) i*(j+2) + 2) / n;
	B[i][j] = ((DATA_TYPE) i*(j+3) + 3) / n;
      }
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
void kernel_jacobi_2d_imper(int tsteps,
			    int n,
			    DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
			    DATA_TYPE POLYBENCH_2D(B,N,N,n,n))
{
  int t, i, j;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c1 = 0; c1 <= 2016; c1 += 32)
    for (int c2 = 0; c2 <= 2016; c2 += 32)
      for (int c3 = ppcg_max(ppcg_max(0, (c1 / 2) - 999), (c2 / 2) - 999); c3 <= ppcg_min(ppcg_min(19, (c1 / 2) + 15), (c2 / 2) + 15); c3 += 1)
        for (int c4 = ppcg_max(0, -c1 + 2 * c3 + 1); c4 <= ppcg_min(ppcg_min(31, -c1 + 2 * c3 + 1999), (c2 / 2) - c1 + c3 + 2013); c4 += 1)
          for (int c5 = ppcg_max(ppcg_max(0, -c2 + 2 * c3 + 1), c1 - c2 + c4 - 1997); c5 <= ppcg_min(ppcg_min(31, -c2 + 2 * c3 + 1999), c1 - c2 + c4 + 1997); c5 += 1) {
            if (c1 + c4 >= 2 * c3 + 2 && c2 + c5 >= 2 * c3 + 2)
              A[c1 - 2 * c3 + c4 - 1][c2 - 2 * c3 + c5 - 1] = B[c1 - 2 * c3 + c4 - 1][c2 - 2 * c3 + c5 - 1];
            if (2 * c3 + 1998 >= c1 + c4 && 2 * c3 + 1998 >= c2 + c5)
              B[c1 - 2 * c3 + c4][c2 - 2 * c3 + c5] = (0.2 * ((((A[c1 - 2 * c3 + c4][c2 - 2 * c3 + c5] + A[c1 - 2 * c3 + c4][c2 - 2 * c3 + c5 - 1]) + A[c1 - 2 * c3 + c4][c2 - 2 * c3 + c5 + 1]) + A[c1 - 2 * c3 + c4 + 1][c2 - 2 * c3 + c5]) + A[c1 - 2 * c3 + c4 - 1][c2 - 2 * c3 + c5]));
          }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(B, DATA_TYPE, N, N, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_jacobi_2d_imper (tsteps, n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(A)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
