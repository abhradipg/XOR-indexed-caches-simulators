

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

void kernel_symm() {
  double C[M][N];
  double A[M][M];
  double B[M][N];
  double alpha;
  double beta;
  double temp2;
  int i, j, k;

#pragma scop
  for (i = 0; i < M; i++)
    for (j = 0; j < N; j++ ) {
      temp2 = 0;
      for (k = 0; k < i; k++) {
        C[k][j] += alpha*B[i][j] * A[i][k];
        temp2 += B[k][j] * A[i][k];
      }
      C[i][j] = beta * C[i][j] + alpha*B[i][j] * A[i][i] + alpha * temp2;
    }
#pragma endscop

}