/**
 * gramschmidt.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 512. */
#include "gramschmidt.h"


/* Array initialization. */
static
void init_array(int ni, int nj,
		DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni,nj),
		DATA_TYPE POLYBENCH_2D(R,NJ,NJ,nj,nj),
		DATA_TYPE POLYBENCH_2D(Q,NI,NJ,ni,nj))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
      A[i][j] = ((DATA_TYPE) i*j) / ni;
      Q[i][j] = ((DATA_TYPE) i*(j+1)) / nj;
    }
  for (i = 0; i < nj; i++)
    for (j = 0; j < nj; j++)
      R[i][j] = ((DATA_TYPE) i*(j+2)) / nj;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nj,
		 DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni,nj),
		 DATA_TYPE POLYBENCH_2D(R,NJ,NJ,nj,nj),
		 DATA_TYPE POLYBENCH_2D(Q,NI,NJ,ni,nj))
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j]);
	if (i % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
  for (i = 0; i < nj; i++)
    for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, R[i][j]);
	if (i % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, Q[i][j]);
	if (i % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_gramschmidt(int ni, int nj,
			DATA_TYPE POLYBENCH_2D(A,NI,NJ,ni,nj),
			DATA_TYPE POLYBENCH_2D(R,NJ,NJ,nj,nj),
			DATA_TYPE POLYBENCH_2D(Q,NI,NJ,ni,nj))
{
  int i, j, k;

  DATA_TYPE nrm;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 1999; c0 += 32)
    for (int c1 = c0; c1 <= 2000; c1 += 32) {
      for (int c2 = 0; c2 <= ppcg_min(ppcg_min(31, -c0 + 1998), -c0 + c1 + 30); c2 += 1) {
        if (c1 == c0) {
          nrm = 0;
          for (int c4 = 0; c4 <= 1999; c4 += 1)
            nrm += (A[c4][c0 + c2] * A[c4][c0 + c2]);
          R[c0 + c2][c0 + c2] = sqrt(nrm);
        }
        for (int c3 = ppcg_max(0, c0 - c1 + c2 + 1); c3 <= ppcg_min(31, -c1 + 1999); c3 += 1) {
          for (int c4 = 0; c4 <= 1999; c4 += 1) {
            if (c1 + c3 == c0 + c2 + 1)
              Q[c4][c0 + c2] = (A[c4][c0 + c2] / R[c0 + c2][c0 + c2]);
            if (c4 == 0)
              R[c0 + c2][c1 + c3] = 0;
            R[c0 + c2][c1 + c3] += (Q[c4][c0 + c2] * A[c4][c1 + c3]);
          }
          for (int c4 = 0; c4 <= 1999; c4 += 1)
            A[c4][c1 + c3] = (A[c4][c1 + c3] - (Q[c4][c0 + c2] * R[c0 + c2][c1 + c3]));
        }
      }
      if (c0 <= 1952 && c1 == c0) {
        nrm = 0;
        for (int c4 = 0; c4 <= 1999; c4 += 1)
          nrm += (A[c4][c0 + 31] * A[c4][c0 + 31]);
        R[c0 + 31][c0 + 31] = sqrt(nrm);
      } else if (c0 == 1984 && c1 == 1984) {
        nrm = 0;
        for (int c4 = 0; c4 <= 1999; c4 += 1)
          nrm += (A[c4][1999] * A[c4][1999]);
        R[1999][1999] = sqrt(nrm);
        for (int c4 = 0; c4 <= 1999; c4 += 1)
          Q[c4][1999] = (A[c4][1999] / R[1999][1999]);
      }
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(A,DATA_TYPE,NI,NJ,ni,nj);
  POLYBENCH_2D_ARRAY_DECL(R,DATA_TYPE,NJ,NJ,nj,nj);
  POLYBENCH_2D_ARRAY_DECL(Q,DATA_TYPE,NI,NJ,ni,nj);

  /* Initialize array(s). */
  init_array (ni, nj,
	      POLYBENCH_ARRAY(A),
	      POLYBENCH_ARRAY(R),
	      POLYBENCH_ARRAY(Q));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_gramschmidt (ni, nj,
		      POLYBENCH_ARRAY(A),
		      POLYBENCH_ARRAY(R),
		      POLYBENCH_ARRAY(Q));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(ni, nj, POLYBENCH_ARRAY(A), POLYBENCH_ARRAY(R), POLYBENCH_ARRAY(Q)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(R);
  POLYBENCH_FREE_ARRAY(Q);

  return 0;
}
