

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define N 30
#  endif

#  ifdef SMALL_DATASET
#   define N 90
#  endif

#  ifdef MEDIUM_DATASET
#   define N 250
#  endif

#  ifdef LARGE_DATASET
#   define N 1300
#  endif

#  ifdef EXTRALARGE_DATASET
#   define N 2800
#  endif

void kernel_gesummv() {
  double A[N][N];
  double B[N][N];
  double tmp[N];
  double x[N];
  double y[N];
  double alpha;
  double beta;
  int i, j;

#pragma scop
  for (i = 0; i < N; i++) {
    tmp[i] = 0.0;
    y[i] = 0.0;
    for (j = 0; j < N; j++) {
      tmp[i] = A[i][j] * x[j] + tmp[i];
      y[i] = B[i][j] * x[j] + y[i];
    }
    y[i] = alpha * tmp[i] + beta * y[i];
  }
#pragma endscop

}