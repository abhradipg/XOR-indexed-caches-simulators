
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define NI 16
#   define NJ 18
#   define NK 22
#   define NL 24
#  endif

#  ifdef SMALL_DATASET
#   define NI 40
#   define NJ 50
#   define NK 70
#   define NL 80
#  endif

#  ifdef MEDIUM_DATASET
#   define NI 180
#   define NJ 190
#   define NK 210
#   define NL 220
#  endif

#  ifdef LARGE_DATASET
#   define NI 800
#   define NJ 900
#   define NK 1100
#   define NL 1200
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NI 1600
#   define NJ 1800
#   define NK 2200
#   define NL 2400
#  endif

void kernel_2mm() {
  double tmp[NI][NJ];
  double A[NI][NK];
  double B[NK][NJ];
  double C[NJ][NL];
  double D[NI][NL];
  double alpha = 1.5;
  double beta = 1.2;  
  int i, j, k;

#pragma scop
  for (i = 0; i < NI; i++)
    for (j = 0; j < NJ; j++) {
      tmp[i][j] = 0.0;
      for (k = 0; k < NK; ++k)
        tmp[i][j] += alpha * A[i][k] * B[k][j];
    }
    for (i = 0; i < NI; i++)
      for (j = 0; j < NL; j++) {
        D[i][j] *= beta;
        for (k = 0; k < NJ; ++k)
          D[i][j] += tmp[i][k] * C[k][j];
      }
#pragma endscop

}
