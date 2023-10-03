/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* adi.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "adi.h"


/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE POLYBENCH_2D(u,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      {
	u[i][j] =  (DATA_TYPE)(i + n-j) / n;
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_2D(u,N,N,n,n))

{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("u");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if ((i * n + j) % 20 == 0) fprintf(POLYBENCH_DUMP_TARGET, "\n");
      fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, u[i][j]);
    }
  POLYBENCH_DUMP_END("u");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* Based on a Fortran code fragment from Figure 5 of
 * "Automatic Data and Computation Decomposition on Distributed Memory Parallel Computers"
 * by Peizong Lee and Zvi Meir Kedem, TOPLAS, 2002
 */
static
__attribute__((noinline)) void kernel_adi(int tsteps, int n,
		DATA_TYPE POLYBENCH_2D(u,N,N,n,n),
		DATA_TYPE POLYBENCH_2D(v,N,N,n,n),
		DATA_TYPE POLYBENCH_2D(p,N,N,n,n),
		DATA_TYPE POLYBENCH_2D(q,N,N,n,n))
{
  int t, i, j;
  DATA_TYPE DX, DY, DT;
  DATA_TYPE B1, B2;
  DATA_TYPE mul1, mul2;
  DATA_TYPE a, b, c, d, e, f;

#pragma scop
  /* ppcg generated CPU code */
  
  #define notused_min(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x < _y ? _x : _y; })
  #define notused_max(x,y)    ({ __typeof__(x) _x = (x); __typeof__(y) _y = (y); _x > _y ? _x : _y; })
  {
    DX = (1. / ((double) 1000));
    DT = (1. / ((double) 500));
    B1 = 2.;
    mul1 = ((B1 * DT) / (DX * DX));
    b = (1. + mul1);
    B2 = 1.;
    DY = (1. / ((double) 1000));
    mul2 = ((B2 * DT) / (DY * DY));
    d = ((-mul2) / 2.);
    f = d;
    e = (1. + mul2);
    a = ((-mul1) / 2.);
    c = a;
    for (int c0 = 1; c0 <= 500; c0 += 1) {
      for (int c1 = 1; c1 <= 998; c1 += 1)
        p[c1][0] = 0.;
      for (int c1 = 0; c1 <= 62; c1 += 1)
        for (int c2 = 0; c2 <= 62; c2 += 1)
          for (int c3 = max(1, 16 * c1); c3 <= min(998, 16 * c1 + 15); c3 += 1)
            for (int c4 = max(1, 16 * c2); c4 <= min(998, 16 * c2 + 15); c4 += 1)
              p[c3][c4] = ((-c) / ((a * p[c3][c4 - 1]) + b));
      for (int c1 = 1; c1 <= 998; c1 += 1)
        v[0][c1] = 1.;
      for (int c1 = 1; c1 <= 998; c1 += 1)
        q[c1][0] = v[0][c1];
      for (int c1 = 0; c1 <= 62; c1 += 1)
        for (int c2 = 0; c2 <= 62; c2 += 1)
          for (int c3 = max(1, 16 * c1); c3 <= min(998, 16 * c1 + 15); c3 += 1)
            for (int c4 = max(1, 16 * c2); c4 <= min(998, 16 * c2 + 15); c4 += 1)
              q[c3][c4] = ((((((-d) * u[c4][c3 - 1]) + ((1. + (2. * d)) * u[c4][c3])) - (f * u[c4][c3 + 1])) - (a * q[c3][c4 - 1])) / ((a * p[c3][c4 - 1]) + b));
      for (int c1 = 1; c1 <= 998; c1 += 1)
        u[c1][0] = 1.;
      for (int c1 = 1; c1 <= 998; c1 += 1)
        v[999][c1] = 1.;
      for (int c1 = 1; c1 <= 998; c1 += 1)
        q[c1][0] = u[c1][0];
      for (int c1 = 1; c1 <= 998; c1 += 1)
        for (int c2 = -998; c2 < 0; c2 += 1)
          v[-c2][c1] = ((p[c1][-c2] * v[-c2 + 1][c1]) + q[c1][-c2]);
      for (int c1 = 1; c1 <= 998; c1 += 1)
        p[c1][0] = 0.;
      for (int c1 = 0; c1 <= 62; c1 += 1)
        for (int c2 = 0; c2 <= 62; c2 += 1)
          for (int c3 = max(1, 16 * c1); c3 <= min(998, 16 * c1 + 15); c3 += 1)
            for (int c4 = max(1, 16 * c2); c4 <= min(998, 16 * c2 + 15); c4 += 1)
              p[c3][c4] = ((-f) / ((d * p[c3][c4 - 1]) + e));
      for (int c1 = 0; c1 <= 62; c1 += 1)
        for (int c2 = 0; c2 <= 62; c2 += 1)
          for (int c3 = max(1, 16 * c1); c3 <= min(998, 16 * c1 + 15); c3 += 1)
            for (int c4 = max(1, 16 * c2); c4 <= min(998, 16 * c2 + 15); c4 += 1)
              q[c3][c4] = ((((((-a) * v[c3 - 1][c4]) + ((1. + (2. * a)) * v[c3][c4])) - (c * v[c3 + 1][c4])) - (d * q[c3][c4 - 1])) / ((d * p[c3][c4 - 1]) + e));
      for (int c1 = 1; c1 <= 998; c1 += 1)
        u[c1][999] = 1.;
      for (int c1 = 1; c1 <= 998; c1 += 1)
        for (int c2 = -998; c2 < 0; c2 += 1)
          u[c1][-c2] = ((p[c1][-c2] * u[c1][-c2 + 1]) + q[c1][-c2]);
    }
  }
#pragma endscop
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(u, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(v, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(p, DATA_TYPE, N, N, n, n);
  POLYBENCH_2D_ARRAY_DECL(q, DATA_TYPE, N, N, n, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(u));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_adi (tsteps, n, POLYBENCH_ARRAY(u), POLYBENCH_ARRAY(v), POLYBENCH_ARRAY(p), POLYBENCH_ARRAY(q));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(u)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(u);
  POLYBENCH_FREE_ARRAY(v);
  POLYBENCH_FREE_ARRAY(p);
  POLYBENCH_FREE_ARRAY(q);

  return 0;
}
