

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define N 40
#  endif

#  ifdef SMALL_DATASET
#   define N 120
#  endif

#  ifdef MEDIUM_DATASET
#   define N 400
#  endif

#  ifdef LARGE_DATASET
#   define N 2000
#  endif

#  ifdef EXTRALARGE_DATASET
#   define N 4000
#  endif

void kernel_ludcmp() {
  double A[N][N];
  double b[N];
  double x[N];
  double y[N];
  double w;
  int i, j, k;

#pragma scop
  for (i = 0; i < N; i++) {
    for (j = 0; j <i; j++) {
      w = A[i][j];
      for (k = 0; k < j; k++) {
        w -= A[i][k] * A[k][j];
      }
      A[i][j] = w / A[j][j];
    }
    for (j = i; j < N; j++) {
      w = A[i][j];
      for (k = 0; k < i; k++) {
        w -= A[i][k] * A[k][j];
      }
      A[i][j] = w;
    }
  }

  for (i = 0; i < N; i++) {
    w = b[i];
    for (j = 0; j < i; j++)
      w -= A[i][j] * y[j];
    y[i] = w;
  }

  for (i = N -1; i >=0; i--) {
    w = y[i];
    for (j = i+1; j < N; j++)
      w -= A[i][j] * x[j];
    x[i] = w / A[i][i];
  }
#pragma endscop

}