/**
 * symm.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "symm.h"


/* Array initialization. */
static
void init_array(int ni, int nj,
		DATA_TYPE *alpha,
		DATA_TYPE *beta,
		DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj),
		DATA_TYPE POLYBENCH_2D(A,NJ,NJ,nj,nj),
		DATA_TYPE POLYBENCH_2D(B,NI,NJ,ni,nj))
{
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
      C[i][j] = ((DATA_TYPE) i*j) / ni;
      B[i][j] = ((DATA_TYPE) i*j) / ni;
    }
  for (i = 0; i < nj; i++)
    for (j = 0; j < nj; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nj,
		 DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, C[i][j]);
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_symm(int ni, int nj,
		 DATA_TYPE alpha,
		 DATA_TYPE beta,
		 DATA_TYPE POLYBENCH_2D(C,NI,NJ,ni,nj),
		 DATA_TYPE POLYBENCH_2D(A,NJ,NJ,nj,nj),
		 DATA_TYPE POLYBENCH_2D(B,NI,NJ,ni,nj))
{
  int i, j, k;
  DATA_TYPE acc;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 1999; c0 += 32)
    for (int c1 = 0; c1 <= 1999; c1 += 32)
      for (int c2 = 0; c2 <= ppcg_min(31, -c0 + 1999); c2 += 1)
        for (int c3 = 0; c3 <= ppcg_min(31, -c1 + 1999); c3 += 1) {
          acc = 0;
          for (int c4 = 0; c4 < c1 + c3 - 1; c4 += 1) {
            if (c1 + c3 >= c0 + c2 + 2 && c0 + c2 + c4 >= 1997)
              C[c0 + c2][c1 + c3] += ((alpha * A[c0 + c2][c0 + c2 + c4 - 1997]) * B[c0 + c2 + c4 - 1997][c1 + c3]);
            acc += (B[c4][c1 + c3] * A[c4][c0 + c2]);
            if (c1 == 1984 && c3 == 15 && c4 == 1997)
              C[c0 + c2][1999] = (((beta * C[c0 + c2][1999]) + ((alpha * A[c0 + c2][c0 + c2]) * B[c0 + c2][1999])) + (alpha * acc));
          }
          if (c1 + c3 >= c0 + c2 + 2)
            for (int c4 = ppcg_max(-c0 - c2 + 1997, c1 + c3 - 1); c4 <= 1996; c4 += 1)
              C[c0 + c2][c1 + c3] += ((alpha * A[c0 + c2][c0 + c2 + c4 - 1997]) * B[c0 + c2 + c4 - 1997][c1 + c3]);
          if (c1 + c3 <= 1998) {
            if (c1 + c3 >= c0 + c2 + 2)
              C[c0 + c2][c1 + c3] += ((alpha * A[c0 + c2][c0 + c2]) * B[c0 + c2][c1 + c3]);
            C[c0 + c2][c1 + c3] = (((beta * C[c0 + c2][c1 + c3]) + ((alpha * A[c0 + c2][c0 + c2]) * B[c0 + c2][c1 + c3])) + (alpha * acc));
          }
          if (c1 + c3 >= c0 + c2 + 2)
            for (int c4 = 1998; c4 <= -c0 - c2 + 3996; c4 += 1)
              C[c0 + c2][c1 + c3] += ((alpha * A[c0 + c2][c0 + c2 + c4 - 1997]) * B[c0 + c2 + c4 - 1997][c1 + c3]);
        }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,NI,NJ,ni,nj);
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NJ,NJ,nj,nj);
  POLYBENCH_2D_ARRAY_DECL(B,DATA_TYPE,NI,NJ,ni,nj);

  /* Initialize array(s). */
  init_array (ni, nj, &alpha, &beta,
	      POLYBENCH_ARRAY(C),
	      POLYBENCH_ARRAY(A),
	      POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_symm (ni, nj,
	       alpha, beta,
	       POLYBENCH_ARRAY(C),
	       POLYBENCH_ARRAY(A),
	       POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(ni, nj,  POLYBENCH_ARRAY(C)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
