/**
 * doitgen.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "doitgen.h"


/* Array initialization. */
static
void init_array(int nr, int nq, int np,
		DATA_TYPE POLYBENCH_3D(A,NR,NQ,NP,nr,nq,np),
		DATA_TYPE POLYBENCH_2D(C4,NP,NP,np,np))
{
  int i, j, k;

  for (i = 0; i < nr; i++)
    for (j = 0; j < nq; j++)
      for (k = 0; k < np; k++)
	A[i][j][k] = ((DATA_TYPE) i*j + k) / np;
  for (i = 0; i < np; i++)
    for (j = 0; j < np; j++)
      C4[i][j] = ((DATA_TYPE) i*j) / np;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int nr, int nq, int np,
		 DATA_TYPE POLYBENCH_3D(A,NR,NQ,NP,nr,nq,np))
{
  int i, j, k;

  for (i = 0; i < nr; i++)
    for (j = 0; j < nq; j++)
      for (k = 0; k < np; k++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j][k]);
	if (i % 20 == 0) fprintf (stderr, "\n");
      }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_doitgen(int nr, int nq, int np,
		    DATA_TYPE POLYBENCH_3D(A,NR,NQ,NP,nr,nq,np),
		    DATA_TYPE POLYBENCH_2D(C4,NP,NP,np,np),
		    DATA_TYPE POLYBENCH_3D(sum,NR,NQ,NP,nr,nq,np))
{
  int r, q, p, s;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 255; c0 += 32)
    for (int c1 = 0; c1 <= 255; c1 += 32)
      for (int c2 = 0; c2 <= 510; c2 += 32)
        for (int c3 = ppcg_max(0, c2 - 256); c3 <= ppcg_min(255, c2 + 31); c3 += 32)
          for (int c4 = 0; c4 <= 31; c4 += 1)
            for (int c5 = 0; c5 <= 31; c5 += 1) {
              if (c2 >= c3 + 32 && c3 <= 192) {
                for (int c6 = 0; c6 <= ppcg_min(31, -c2 + c3 + 286); c6 += 1)
                  for (int c7 = ppcg_max(0, c2 - c3 + c6 - 255); c7 <= 31; c7 += 1)
                    sum[c0 + c4][c1 + c5][c3 + c7] = (sum[c0 + c4][c1 + c5][c3 + c7] + (A[c0 + c4][c1 + c5][c2 - c3 + c6 - c7] * C4[c2 - c3 + c6 - c7][c3 + c7]));
              } else if (c3 == c2) {
                for (int c6 = 0; c6 <= 31; c6 += 1) {
                  for (int c7 = 0; c7 <= c6; c7 += 1) {
                    if (c7 == c6)
                      sum[c0 + c4][c1 + c5][c2 + c6] = 0;
                    sum[c0 + c4][c1 + c5][c2 + c7] = (sum[c0 + c4][c1 + c5][c2 + c7] + (A[c0 + c4][c1 + c5][c6 - c7] * C4[c6 - c7][c2 + c7]));
                  }
                  if (c2 == 224 && c6 == 31)
                    A[c0 + c4][c1 + c5][0] = sum[c0 + c4][c1 + c5][0];
                }
              } else {
                for (int c6 = 0; c6 <= ppcg_min(31, -c2 + 510); c6 += 1) {
                  for (int c7 = ppcg_max(0, c2 + c6 - 479); c7 <= 31; c7 += 1)
                    sum[c0 + c4][c1 + c5][c7 + 224] = (sum[c0 + c4][c1 + c5][c7 + 224] + (A[c0 + c4][c1 + c5][c2 + c6 - c7 - 224] * C4[c2 + c6 - c7 - 224][c7 + 224]));
                  A[c0 + c4][c1 + c5][c2 + c6 - 255] = sum[c0 + c4][c1 + c5][c2 + c6 - 255];
                }
              }
            }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int nr = NR;
  int nq = NQ;
  int np = NP;

  /* Variable declaration/allocation. */
  POLYBENCH_3D_ARRAY_DECL(A,DATA_TYPE,NR,NQ,NP,nr,nq,np);
  POLYBENCH_3D_ARRAY_DECL(sum,DATA_TYPE,NR,NQ,NP,nr,nq,np);
  POLYBENCH_2D_ARRAY_DECL(C4,DATA_TYPE,NP,NP,np,np);

  /* Initialize array(s). */
  init_array (nr, nq, np,
	      POLYBENCH_ARRAY(A),
	      POLYBENCH_ARRAY(C4));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_doitgen (nr, nq, np,
		  POLYBENCH_ARRAY(A),
		  POLYBENCH_ARRAY(C4),
		  POLYBENCH_ARRAY(sum));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(nr, nq, np,  POLYBENCH_ARRAY(A)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(sum);
  POLYBENCH_FREE_ARRAY(C4);

  return 0;
}
