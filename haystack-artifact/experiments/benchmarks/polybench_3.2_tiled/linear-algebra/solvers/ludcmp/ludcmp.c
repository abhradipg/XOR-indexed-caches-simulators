/**
 * ludcmp.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "ludcmp.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(A,N+1,N+1,n+1,n+1),
		 DATA_TYPE POLYBENCH_1D(b,N+1,n+1),
		 DATA_TYPE POLYBENCH_1D(x,N+1,n+1),
		 DATA_TYPE POLYBENCH_1D(y,N+1,n+1))
{
  int i, j;

  for (i = 0; i <= n; i++)
    {
      x[i] = i + 1;
      y[i] = (i+1)/n/2.0 + 1;
      b[i] = (i+1)/n/2.0 + 42;
      for (j = 0; j <= n; j++) {
	A[i][j] = ((DATA_TYPE) (i+1)*(j+1)) / n;
      }
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_1D(x,N+1,n+1))

{
  int i;

  for (i = 0; i <= n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, x[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_ludcmp(int n,
		   DATA_TYPE POLYBENCH_2D(A,N+1,N+1,n+1,n+1),
		   DATA_TYPE POLYBENCH_1D(b,N+1,n+1),
		   DATA_TYPE POLYBENCH_1D(x,N+1,n+1),
		   DATA_TYPE POLYBENCH_1D(y,N+1,n+1))
{
  int i, j, k;

  DATA_TYPE w;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  {
    for (int c0 = 0; c0 <= 1999; c0 += 32)
      for (int c1 = c0; c1 <= 2000; c1 += 32)
        for (int c2 = 0; c2 <= ppcg_min(ppcg_min(31, -c0 + 1999), -c0 + c1 + 30); c2 += 1)
          for (int c3 = ppcg_max(0, c0 - c1 + c2 + 1); c3 <= ppcg_min(31, -c1 + 2000); c3 += 1) {
            w = A[c1 + c3][c0 + c2];
            for (int c4 = 0; c4 < c0 + c2; c4 += 1)
              w = (w - (A[c1 + c3][c4] * A[c4][c0 + c2]));
            A[c1 + c3][c0 + c2] = (w / A[c0 + c2][c0 + c2]);
            w = A[c0 + c2 + 1][c1 + c3];
            for (int c4 = 1998; c4 <= c0 + c2 + 1998; c4 += 1)
              w = (w - (A[c0 + c2 + 1][c4 - 1998] * A[c4 - 1998][c1 + c3]));
            A[c0 + c2 + 1][c1 + c3] = w;
          }
    b[0] = 1.0;
    for (int c0 = 1; c0 <= 2000; c0 += 1) {
      w = b[c0];
      if (c0 == 1)
        y[0] = b[0];
      for (int c1 = 0; c1 < c0; c1 += 1)
        w = (w - (A[c0][c1] * y[c1]));
      y[c0] = w;
    }
    for (int c0 = 2000; c0 <= 4000; c0 += 1) {
      if (c0 >= 2001)
        x[-c0 + 4000] = (w / A[-c0 + 4000][-c0 + 4000]);
      if (c0 <= 3999) {
        w = y[-c0 + 3999];
        if (c0 == 2000)
          x[2000] = (y[2000] / A[2000][2000]);
        for (int c1 = -c0 + 4000; c1 <= 2000; c1 += 1)
          w = (w - (A[-c0 + 3999][c1] * x[c1]));
      }
    }
  }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N+1, N+1, n+1, n+1);
  POLYBENCH_1D_ARRAY_DECL(b, DATA_TYPE, N+1, n+1);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N+1, n+1);
  POLYBENCH_1D_ARRAY_DECL(y, DATA_TYPE, N+1, n+1);


  /* Initialize array(s). */
  init_array (n,
	      POLYBENCH_ARRAY(A),
	      POLYBENCH_ARRAY(b),
	      POLYBENCH_ARRAY(x),
	      POLYBENCH_ARRAY(y));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_ludcmp (n,
		 POLYBENCH_ARRAY(A),
		 POLYBENCH_ARRAY(b),
		 POLYBENCH_ARRAY(x),
		 POLYBENCH_ARRAY(y));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(x)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(b);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(y);

  return 0;
}
