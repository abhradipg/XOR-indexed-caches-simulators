

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TSTEPS 20
#   define N 10
#  endif

#  ifdef SMALL_DATASET
#   define TSTEPS 40
#   define N 20
#  endif

#  ifdef MEDIUM_DATASET
#   define TSTEPS 100
#   define N 40
#  endif

#  ifdef LARGE_DATASET
#   define TSTEPS 500
#   define N 120
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TSTEPS 1000
#   define N 200
#  endif

void kernel_heat_3d() {
  double A[N][N][N];
  double B[N][N][N];
  int t, i, j, k;

#pragma scop
  for (t = 1; t <= TSTEPS; t++) {
    for (i = 1; i < N - 1; i++) {
      for (j = 1; j < N - 1; j++) {
        for (k = 1; k < N - 1; k++) {
          B[i][j][k] = 0.125 * (A[i+1][j][k] - 2.0 * A[i][j][k] + A[i-1][j][k])
                       + 0.125 * (A[i][j+1][k] - 2.0 * A[i][j][k] + A[i][j-1][k])
                       + 0.125 * (A[i][j][k+1] - 2.0 * A[i][j][k] + A[i][j][k-1])
                       + A[i][j][k];
        }
      }
    }
    for (i = 1; i < N - 1; i++) {
      for (j = 1; j < N - 1; j++) {
        for (k = 1; k < N - 1; k++) {
          A[i][j][k] = 0.125 * (B[i+1][j][k] - 2.0 * B[i][j][k] + B[i-1][j][k])
                       + 0.125 * (B[i][j+1][k] - 2.0 * B[i][j][k] + B[i][j-1][k])
                       + 0.125 * (B[i][j][k+1] - 2.0 * B[i][j][k] + B[i][j][k-1])
                       + B[i][j][k];
        }
      }
    }
  }
#pragma endscop

}