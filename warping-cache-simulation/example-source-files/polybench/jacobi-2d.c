

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TSTEPS 20
#   define N 30
#  endif

#  ifdef SMALL_DATASET
#   define TSTEPS 40
#   define N 90
#  endif

#  ifdef MEDIUM_DATASET
#   define TSTEPS 100
#   define N 250
#  endif

#  ifdef LARGE_DATASET
#   define TSTEPS 500
#   define N 1300
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TSTEPS 1000
#   define N 2800
#  endif

void kernel_jacobi_2d() {
  double A[N][N];
  double B[N][N];
  int t, i, j;

#pragma scop
  for (t = 0; t < TSTEPS; t++) {
    for (i = 1; i < N - 1; i++)
      for (j = 1; j < N - 1; j++)
        B[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i][1+j] + A[1+i][j] + A[i-1][j]);
    for (i = 1; i < N - 1; i++)
      for (j = 1; j < N - 1; j++)
        A[i][j] = 0.2 * (B[i][j] + B[i][j-1] + B[i][1+j] + B[1+i][j] + B[i-1][j]);
  }
#pragma endscop

}