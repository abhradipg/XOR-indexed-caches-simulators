/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 10x1024x1024. */
#include "adi.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(X,N,N,n,n),
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
		 DATA_TYPE POLYBENCH_2D(B,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      {
	X[i][j] = ((DATA_TYPE) i*(j+1) + 1) / n;
	A[i][j] = ((DATA_TYPE) i*(j+2) + 2) / n;
	B[i][j] = ((DATA_TYPE) i*(j+3) + 3) / n;
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_2D(X,N,N,n,n))

{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, X[i][j]);
      if ((i * N + j) % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_adi(int tsteps,
		int n,
		DATA_TYPE POLYBENCH_2D(X,N,N,n,n),
		DATA_TYPE POLYBENCH_2D(A,N,N,n,n),
		DATA_TYPE POLYBENCH_2D(B,N,N,n,n))
{
  int t, i1, i2;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 49; c0 += 1) {
    for (int c1 = 0; c1 <= 1999; c1 += 32)
      for (int c2 = 0; c2 <= 1999; c2 += 32)
        for (int c3 = 0; c3 <= ppcg_min(31, -c1 + 1999); c3 += 1) {
          for (int c4 = ppcg_max(0, -c2 + 1); c4 <= ppcg_min(31, -c2 + 1999); c4 += 1) {
            B[c1 + c3][c2 + c4] = (B[c1 + c3][c2 + c4] - ((A[c1 + c3][c2 + c4] * A[c1 + c3][c2 + c4]) / B[c1 + c3][c2 + c4 - 1]));
            X[c1 + c3][c2 + c4] = (X[c1 + c3][c2 + c4] - ((X[c1 + c3][c2 + c4 - 1] * A[c1 + c3][c2 + c4]) / B[c1 + c3][c2 + c4 - 1]));
          }
          if (c2 == 1984)
            X[c1 + c3][1999] = (X[c1 + c3][1999] / B[c1 + c3][1999]);
        }
    for (int c1 = -32; c1 <= 1998; c1 += 32)
      for (int c2 = 0; c2 <= 1999; c2 += 32)
        for (int c3 = ppcg_max(0, -c1 - 1); c3 <= ppcg_min(31, -c1 + 1998); c3 += 1) {
          if (c1 + c3 >= 0 && c1 + c3 <= 1997) {
            for (int c4 = 0; c4 <= ppcg_min(31, -c2 + 1999); c4 += 1) {
              if (c2 + c4 >= 1)
                B[c2 + c4][-c1 - c3 + 1998] = (B[c2 + c4][-c1 - c3 + 1998] - ((A[c2 + c4][-c1 - c3 + 1998] * A[c2 + c4][-c1 - c3 + 1998]) / B[c2 + c4 - 1][-c1 - c3 + 1998]));
              X[c2 + c4][-c1 - c3 + 1998] = ((X[c2 + c4][-c1 - c3 + 1998] - (X[c2 + c4][-c1 - c3 + 1997] * A[c2 + c4][-c1 - c3 + 1997])) / B[c2 + c4][-c1 - c3 + 1997]);
              if (c2 + c4 >= 1)
                X[c2 + c4][-c1 - c3 + 1998] = (X[c2 + c4][-c1 - c3 + 1998] - ((X[c2 + c4 - 1][-c1 - c3 + 1998] * A[c2 + c4][-c1 - c3 + 1998]) / B[c2 + c4 - 1][-c1 - c3 + 1998]));
            }
          } else if (c1 == -32 && c3 == 31) {
            for (int c4 = ppcg_max(0, -c2 + 1); c4 <= ppcg_min(31, -c2 + 1999); c4 += 1) {
              B[c2 + c4][1999] = (B[c2 + c4][1999] - ((A[c2 + c4][1999] * A[c2 + c4][1999]) / B[c2 + c4 - 1][1999]));
              X[c2 + c4][1999] = (X[c2 + c4][1999] - ((X[c2 + c4 - 1][1999] * A[c2 + c4][1999]) / B[c2 + c4 - 1][1999]));
            }
          } else {
            for (int c4 = ppcg_max(0, -c2 + 1); c4 <= ppcg_min(31, -c2 + 1999); c4 += 1) {
              B[c2 + c4][0] = (B[c2 + c4][0] - ((A[c2 + c4][0] * A[c2 + c4][0]) / B[c2 + c4 - 1][0]));
              X[c2 + c4][0] = (X[c2 + c4][0] - ((X[c2 + c4 - 1][0] * A[c2 + c4][0]) / B[c2 + c4 - 1][0]));
            }
          }
          if (c2 == 1984)
            X[1999][-c1 - c3 + 1998] = (X[1999][-c1 - c3 + 1998] / B[1999][-c1 - c3 + 1998]);
        }
    for (int c1 = 0; c1 <= 1997; c1 += 32)
      for (int c2 = 0; c2 <= 1999; c2 += 32)
        for (int c3 = 0; c3 <= ppcg_min(31, -c1 + 1997); c3 += 1)
          for (int c4 = 0; c4 <= ppcg_min(31, -c2 + 1999); c4 += 1)
            X[-c1 - c3 + 1998][c2 + c4] = ((X[-c1 - c3 + 1998][c2 + c4] - (X[-c1 - c3 + 1997][c2 + c4] * A[-c1 - c3 + 1997][c2 + c4])) / B[-c1 - c3 + 1998][c2 + c4]);
  }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(X, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(B, DATA_TYPE, N, N, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(X), POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_adi (tsteps, n, POLYBENCH_ARRAY(X),
	      POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(X)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(X);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
