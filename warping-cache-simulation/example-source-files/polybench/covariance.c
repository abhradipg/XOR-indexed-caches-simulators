

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

void kernel_covariance() {
  double data[N][M];
  double cov[M][M];
  double mean[M];
  double float_n;
  int i, j, k;

#pragma scop
    for (j = 0; j < M; j++) {
      mean[j] = 0.0;
      for (i = 0; i < N; i++)
        mean[j] += data[i][j];
      mean[j] /= float_n;
    }

    for (i = 0; i < N; i++)
      for (j = 0; j < M; j++)
        data[i][j] -= mean[j];

    for (i = 0; i < M; i++)
      for (j = i; j < M; j++) {
        cov[i][j] = 0.0;
        for (k = 0; k < N; k++)
          cov[i][j] += data[k][i] * data[k][j];
        cov[i][j] /= (float_n - 1.0);
        cov[j][i] = cov[i][j];
      }
#pragma endscop

}