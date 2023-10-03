/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* cholesky.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "cholesky.h"


/* Array initialization. */
static
void init_array(int n,
		DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      for (j = 0; j <= i; j++)
	A[i][j] = (DATA_TYPE)(-j % n) / n + 1;
      for (j = i+1; j < n; j++) {
	A[i][j] = 0;
      }
      A[i][i] = 1;
    }

  /* Make the matrix positive semi-definite. */
  int r,s,t;
  POLYBENCH_2D_ARRAY_DECL(B, DATA_TYPE, N, N, n, n);
  for (r = 0; r < n; ++r)
    for (s = 0; s < n; ++s)
      (POLYBENCH_ARRAY(B))[r][s] = 0;
  for (t = 0; t < n; ++t)
    for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
	(POLYBENCH_ARRAY(B))[r][s] += A[r][t] * A[s][t];
    for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
	A[r][s] = (POLYBENCH_ARRAY(B))[r][s];
  POLYBENCH_FREE_ARRAY(B);

}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_2D(A,N,N,n,n))

{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("A");
  for (i = 0; i < n; i++)
    for (j = 0; j <= i; j++) {
    if ((i * n + j) % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
    fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, A[i][j]);
  }
  POLYBENCH_DUMP_END("A");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
__attribute__((noinline)) void kernel_cholesky(int n,
		     DATA_TYPE POLYBENCH_2D(A,N,N,n,n))
{
  int i, j, k;


#pragma scop
  /* ppcg generated CPU code */
  
  #define notused_min(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x < _y ? _x : _y; })
  #define notused_max(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x > _y ? _x : _y; })
  for (int c0 = 0; c0 <= 125; c0 += 1)
    for (int c1 = 0; c1 <= min(124, c0); c1 += 1)
      for (int c2 = 0; c2 <= c1; c2 += 1) {
        if (c1 == c0 && c2 == c0) {
          A[16 * c0][16 * c0] = sqrt(A[16 * c0][16 * c0]);
          A[16 * c0 + 1][16 * c0] /= A[16 * c0][16 * c0];
          A[16 * c0 + 1][16 * c0 + 1] -= (A[16 * c0 + 1][16 * c0] * A[16 * c0 + 1][16 * c0]);
        } else if (c1 == c0 && c2 + 1 == c0) {
          A[16 * c0][16 * c0 - 1] /= A[16 * c0 - 1][16 * c0 - 1];
          A[16 * c0][16 * c0] -= (A[16 * c0][16 * c0 - 1] * A[16 * c0][16 * c0 - 1]);
        }
        for (int c3 = max(max(16 * c0, 16 * c1 + 1), 16 * c2 + 2); c3 <= min(1999, 16 * c0 + 15); c3 += 1) {
          for (int c4 = max(16 * c1, 16 * c2 + 1); c4 <= min(16 * c1 + 15, c3 - 1); c4 += 1) {
            for (int c5 = 16 * c2; c5 <= min(16 * c2 + 15, c4 - 1); c5 += 1) {
              if (c5 + 1 == c4) {
                A[c3][c4 - 1] /= A[c4 - 1][c4 - 1];
                A[c3][c3] -= (A[c3][c4 - 1] * A[c3][c4 - 1]);
              }
              A[c3][c4] -= (A[c3][c5] * A[c4][c5]);
            }
            if (c2 == c1 && c4 + 1 == c3)
              A[c3 - 1][c3 - 1] = sqrt(A[c3 - 1][c3 - 1]);
          }
          if (c1 == c0 && c2 == c0) {
            A[c3][c3 - 1] /= A[c3 - 1][c3 - 1];
            A[c3][c3] -= (A[c3][c3 - 1] * A[c3][c3 - 1]);
          }
        }
        if (c0 == 125 && c1 == 124 && c2 == 124)
          A[1999][1999] = sqrt(A[1999][1999]);
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
  kernel_cholesky (n, POLYBENCH_ARRAY(A));

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
