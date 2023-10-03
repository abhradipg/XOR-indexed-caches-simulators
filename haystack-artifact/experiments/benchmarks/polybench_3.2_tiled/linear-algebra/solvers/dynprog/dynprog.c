/**
 * dynprog.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is int, default size is 50. */
#include "dynprog.h"


/* Array initialization. */
static
void init_array(int length,
		DATA_TYPE POLYBENCH_2D(c,LENGTH,LENGTH,length,length),
		DATA_TYPE POLYBENCH_2D(W,LENGTH,LENGTH,length,length))
{
  int i, j;
  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      c[i][j] = i*j % 2;
      W[i][j] = ((DATA_TYPE) i-j) / length;
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(DATA_TYPE out)
{
  fprintf (stderr, DATA_PRINTF_MODIFIER, out);
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_dynprog(int tsteps, int length,
		    DATA_TYPE POLYBENCH_2D(c,LENGTH,LENGTH,length,length),
		    DATA_TYPE POLYBENCH_2D(W,LENGTH,LENGTH,length,length),
		    DATA_TYPE POLYBENCH_3D(sum_c,LENGTH,LENGTH,LENGTH,length,length,length),
		    DATA_TYPE *out)
{
  int iter, i, j, k;

  DATA_TYPE out_l = 0;

#pragma scop
  /* ppcg generated CPU code */
  
  #define ppcg_min(x,y)    (x < y ? x : y)
  #define ppcg_max(x,y)    (x > y ? x : y)
  for (int c0 = 0; c0 <= 999; c0 += 1) {
    for (int c1 = 0; c1 <= 498; c1 += 32)
      for (int c2 = c1; c2 <= 499; c2 += 32)
        for (int c3 = 0; c3 <= ppcg_min(ppcg_min(31, -c1 + 498), -c1 + c2 + 30); c3 += 1)
          for (int c4 = ppcg_max(0, c1 - c2 + c3 + 1); c4 <= ppcg_min(31, -c2 + 499); c4 += 1)
            sum_c[c1 + c3][c2 + c4][c1 + c3] = 0;
    for (int c1 = 0; c1 <= 499; c1 += 32)
      for (int c2 = 0; c2 <= 499; c2 += 32)
        for (int c3 = 0; c3 <= ppcg_min(31, -c1 + 499); c3 += 1)
          for (int c4 = 0; c4 <= ppcg_min(31, -c2 + 499); c4 += 1)
            c[c1 + c3][c2 + c4] = 0;
    for (int c1 = 0; c1 <= 498; c1 += 32)
      for (int c2 = c1; c2 <= 500; c2 += 32)
        for (int c3 = c1; c3 <= ppcg_min(499, c2 + 30); c3 += 32) {
          for (int c4 = 0; c4 <= ppcg_min(ppcg_min(ppcg_min(31, -c1 + 497), -c1 + c2 + 29), -c1 + c3 + 30); c4 += 1) {
            for (int c5 = ppcg_max(ppcg_max(0, -c2 + c3 + 1), c1 - c2 + c4 + 2); c5 <= ppcg_min(31, -c2 + 499); c5 += 1)
              for (int c6 = ppcg_max(0, c1 - c3 + c4 + 1); c6 <= ppcg_min(31, c2 - c3 + c5 - 1); c6 += 1) {
                if (c3 + c6 + 1 == c2 + c5)
                  c[c1 + c4][c2 + c5 - 1] = (sum_c[c1 + c4][c2 + c5 - 1][c2 + c5 - 2] + W[c1 + c4][c2 + c5 - 1]);
                sum_c[c1 + c4][c2 + c5][c3 + c6] = ((sum_c[c1 + c4][c2 + c5][c3 + c6 - 1] + c[c1 + c4][c3 + c6]) + c[c3 + c6][c2 + c5]);
              }
            if (c2 == 480 && c3 == 480) {
              c[c1 + c4][499] = (sum_c[c1 + c4][499][498] + W[c1 + c4][499]);
              if (c1 == 0 && c4 == 0)
                out_l += c[0][499];
            }
          }
          if (c1 == 480 && c2 == 480 && c3 == 480)
            c[498][499] = (sum_c[498][499][498] + W[498][499]);
        }
  }
#pragma endscop

  *out = out_l;
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int length = LENGTH;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  DATA_TYPE out;
  POLYBENCH_3D_ARRAY_DECL(sum_c,DATA_TYPE,LENGTH,LENGTH,LENGTH,length,length,length);
  POLYBENCH_2D_ARRAY_DECL(c,DATA_TYPE,LENGTH,LENGTH,length,length);
  POLYBENCH_2D_ARRAY_DECL(W,DATA_TYPE,LENGTH,LENGTH,length,length);

  /* Initialize array(s). */
  init_array (length, POLYBENCH_ARRAY(c), POLYBENCH_ARRAY(W));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_dynprog (tsteps, length,
		  POLYBENCH_ARRAY(c),
		  POLYBENCH_ARRAY(W),
		  POLYBENCH_ARRAY(sum_c),
		  &out);

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(out));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(sum_c);
  POLYBENCH_FREE_ARRAY(c);
  POLYBENCH_FREE_ARRAY(W);

  return 0;
}
