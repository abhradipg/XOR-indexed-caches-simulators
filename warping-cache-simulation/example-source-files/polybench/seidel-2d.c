

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TSTEPS 20
#   define N 40
#  endif

#  ifdef SMALL_DATASET
#   define TSTEPS 40
#   define N 120
#  endif

#  ifdef MEDIUM_DATASET
#   define TSTEPS 100
#   define N 400
#  endif

#  ifdef LARGE_DATASET
#   define TSTEPS 500
#   define N 2000
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TSTEPS 1000
#   define N 4000
#  endif

void kernel_seidel_2d() {
  double A[N][N];
  int t, i, j;

#pragma scop
  for (t = 0; t <= TSTEPS - 1; t++)
    for (i = 1; i <= N - 2; i++)
      for (j = 1; j <= N - 2; j++)
        A[i][j] = (A[i-1][j-1] + A[i-1][j] + A[i-1][j+1]
                   + A[i][j-1] + A[i][j] + A[i][j+1]
                   + A[i+1][j-1] + A[i+1][j] + A[i+1][j+1])/9.0;
#pragma endscop

}