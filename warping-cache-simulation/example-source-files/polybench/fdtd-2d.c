
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TMAX 20
#   define NX 20
#   define NY 30
#  endif

#  ifdef SMALL_DATASET
#   define TMAX 40
#   define NX 60
#   define NY 80
#  endif

#  ifdef MEDIUM_DATASET
#   define TMAX 100
#   define NX 200
#   define NY 240
#  endif

#  ifdef LARGE_DATASET
#   define TMAX 500
#   define NX 1000
#   define NY 1200
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TMAX 1000
#   define NX 2000
#   define NY 2600
#  endif

void kernel_fdtd_2d() {
  double ex[NX][NY];
  double ey[NX][NY];
  double hz[NX][NY];
  double _fict_[TMAX];
  int t, i, j;

#pragma scop
    for(t = 0; t < TMAX; t++)
    {
      for (j = 0; j < NY; j++)
        ey[0][j] = _fict_[t];
      for (i = 1; i < NX; i++)
        for (j = 0; j < NY; j++)
          ey[i][j] = ey[i][j] - 0.5*(hz[i][j]-hz[i-1][j]);
      for (i = 0; i < NX; i++)
        for (j = 1; j < NY; j++)
          ex[i][j] = ex[i][j] - 0.5*(hz[i][j]-hz[i][j-1]);
      for (i = 0; i < NX - 1; i++)
        for (j = 0; j < NY - 1; j++)
          hz[i][j] = hz[i][j] - 0.7* (ex[i][j+1] - ex[i][j] +
                                      ey[i+1][j] - ey[i][j]);
    }

#pragma endscop

}