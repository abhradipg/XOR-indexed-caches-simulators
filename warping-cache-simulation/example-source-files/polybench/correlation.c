

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define M 28
#   define N 32
#  endif

#  ifdef SMALL_DATASET
#   define M 80
#   define N 100
#  endif

#  ifdef MEDIUM_DATASET
#   define M 240
#   define N 260
#  endif

#  ifdef LARGE_DATASET
#   define M 1200
#   define N 1400
#  endif

#  ifdef EXTRALARGE_DATASET
#   define M 2600
#   define N 3000
#  endif

#include <math.h>

void kernel_correlation() {
  double data[N][M];
  double corr[M][M];
  double mean[M];
  double stddev[M];
  double float_n = (double) N;
  double eps = 0.1;
  int i, j, k;

#pragma scop
    for (j = 0; j < M ; j++) {
      mean[j] = 0.0;
      for (i = 0; i < N; i++)
        mean[j] += data[i][j];
      mean[j] /= float_n;
    }

    for (j = 0; j < M; j++) {
      stddev[j] = 0.0;
      for (i = 0; i < N; i++)
        stddev[j] += (data[i][j] - mean[j]) * (data[i][j] - mean[j]);
      stddev[j] /= float_n;
      stddev[j] = sqrt(stddev[j]);
      stddev[j] = stddev[j] <= eps ? 1.0 : stddev[j];
    }

    for (i = 0; i < N; i++)
      for (j = 0; j < M; j++) {
        data[i][j] -= mean[j];
        data[i][j] /= sqrt(float_n) * stddev[j];
      }

    for (i = 0; i < M -1; i++) {
      corr[i][i] = 1.0;
      for (j = i+1; j < M; j++) {
        corr[i][j] = 0.0;
        for (k = 0; k < N; k++)
          corr[i][j] += (data[k][i] * data[k][j]);
        corr[j][i] = corr[i][j];
      }
    }
    corr[M -1][M -1] = 1.0;
#pragma endscop

}