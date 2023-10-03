
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define M 20
#   define N 30
#  endif

#  ifdef SMALL_DATASET
#   define M 60
#   define N 80
#  endif

#  ifdef MEDIUM_DATASET
#   define M 200
#   define N 240
#  endif

#  ifdef LARGE_DATASET
#   define M 1000
#   define N 1200
#  endif

#  ifdef EXTRALARGE_DATASET
#   define M 2000
#   define N 2600
#  endif

#include <math.h>

void kernel_gramschmidt() {
  double A[M][N];
  double R[N][N];
  double Q[M][N];
  double nrm;
  int i, j, k;

#pragma scop
    for (k = 0; k < N; k++) {
      nrm = 0.0;
      for (i = 0; i < M; i++)
        nrm += A[i][k] * A[i][k];
      R[k][k] = sqrt(nrm);
      for (i = 0; i < M; i++)
        Q[i][k] = A[i][k] / R[k][k];
      for (j = k + 1; j < N; j++) {
        R[k][j] = 0.0;
        for (i = 0; i < M; i++)
          R[k][j] += Q[i][k] * A[i][j];
        for (i = 0; i < M; i++)
          A[i][j] = A[i][j] - Q[i][k] * R[k][j];
      }
    }
#pragma endscop

}