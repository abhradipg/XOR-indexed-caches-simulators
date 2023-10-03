
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define N 60
#  endif

#  ifdef SMALL_DATASET
#   define N 180
#  endif

#  ifdef MEDIUM_DATASET
#   define N 500
#  endif

#  ifdef LARGE_DATASET
#   define N 2800
#  endif

#  ifdef EXTRALARGE_DATASET
#   define N 5600
#  endif

void kernel_floyd_warshall() {
  int path[N][N];
  int i, j, k;

#pragma scop
    for (k = 0; k < N; k++) {
      for(i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
          path[i][j] = path[i][j] < path[i][k] + path[k][j] ?
                       path[i][j] : path[i][k] + path[k][j];
        }
    }
#pragma endscop

}