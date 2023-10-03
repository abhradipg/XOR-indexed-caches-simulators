/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "trisolv.h"


/* Array initialization. */
static
void init_array(int n,
		DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
		DATA_TYPE POLYBENCH_1D(x,N,n),
		DATA_TYPE POLYBENCH_1D(c,N,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      c[i] = x[i] = ((DATA_TYPE) i) / n;
      for (j = 0; j < n; j++)
	A[i][j] = ((DATA_TYPE) i*j) / n;
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_1D(x,N,n))

{
  int i;

  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, x[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_trisolv(int n,
		    DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
		    DATA_TYPE POLYBENCH_1D(x,N,n),
		    DATA_TYPE POLYBENCH_1D(c,N,n))
{
  int i, j;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 8000; c0 += 32)
    for (int c1 = c0; c1 <= 15999; c1 += 32) {
      if (c0 == 0 && c1 >= 32 && c1 <= 7968)
        for (int c3 = 0; c3 <= 31; c3 += 1)
          x[c1 + c3] = c[c1 + c3];
      for (int c2 = ppcg_max(0, (c1 / 2) - c0 + 1); c2 <= ppcg_min(31, -c0 + 7999); c2 += 1) {
        if (c0 == 0 && c1 == 0 && c2 == 1)
          x[0] = c[0];
        for (int c3 = ppcg_max(0, c0 - c1 + c2); c3 <= ppcg_min(31, 2 * c0 - c1 + 2 * c2 - 1); c3 += 1) {
          if (c1 + c3 + 1 == 2 * c0 + 2 * c2) {
            x[c0 + c2 - 1] = (x[c0 + c2 - 1] / A[c0 + c2 - 1][c0 + c2 - 1]);
            if (c0 == 0 && c1 == 0 && c2 == 1)
              x[1] = c[1];
          }
          x[c0 + c2] = (x[c0 + c2] - (A[c0 + c2][-c0 + c1 - c2 + c3] * x[-c0 + c1 - c2 + c3]));
        }
        if (c0 == 0 && c1 == 0 && c2 == 1)
          for (int c3 = 2; c3 <= 31; c3 += 1)
            x[c3] = c[c3];
      }
      if (c0 == 8000 && c1 == 15968)
        x[7999] = (x[7999] / A[7999][7999]);
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(c, DATA_TYPE, N, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(c));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_trisolv (n, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(c));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(x)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(c);

  return 0;
}
