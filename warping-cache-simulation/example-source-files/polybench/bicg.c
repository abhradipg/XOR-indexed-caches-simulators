

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define M 38
#   define N 42
#  endif

#  ifdef SMALL_DATASET
#   define M 116
#   define N 124
#  endif

#  ifdef MEDIUM_DATASET
#   define M 390
#   define N 410
#  endif

#  ifdef LARGE_DATASET
#   define M 1900
#   define N 2100
#  endif

#  ifdef EXTRALARGE_DATASET
#   define M 1800
#   define N 2200
#  endif

void kernel_bicg() {
  double A[N][M];
  double s[M];
  double q[N];
  double p[M];
  double r[N];
  int i, j;

#pragma scop
    for (i = 0; i < M; i++)
      s[i] = 0;
    for (i = 0; i < N; i++) {
      q[i] = 0.0;
      for (j = 0; j < M; j++) {
        s[j] = s[j] + r[i] * A[i][j];
        q[i] = q[i] + A[i][j] * p[j];
      }
    }
#pragma endscop

}