
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
#   define N 2500
#  endif

#  ifdef EXTRALARGE_DATASET
#   define N 5500
#  endif

void kernel_nussinov() {
  double seq[N];
  double table[N][N];
  int i, j, k;

#pragma scop
    for (i = N -1; i >= 0; i--) {
      for (j=i+1; j<N; j++) {

        if (j-1>=0)
          table[i][j] = ((table[i][j] >= table[i][j-1]) ? table[i][j] : table[i][j-1]);
        if (i+1<N)
          table[i][j] = ((table[i][j] >= table[i+1][j]) ? table[i][j] : table[i+1][j]);

        if (j-1>=0 && i+1< N) {
          if (i<j-1)
            table[i][j] = ((table[i][j] >= table[i+1][j-1]+(((seq[i])+(seq[j])) == 3 ? 1 : 0)) ? table[i][j] : table[i+1][j-1]+(((seq[i])+(seq[j])) == 3 ? 1 : 0));
          else
            table[i][j] = ((table[i][j] >= table[i+1][j-1]) ? table[i][j] : table[i+1][j-1]);
        }

        for (k=i+1; k<j; k++) {
          table[i][j] = ((table[i][j] >= table[i][k] + table[k+1][j]) ? table[i][j] : table[i][k] + table[k+1][j]);
        }
      }
    }
#pragma endscop

}
