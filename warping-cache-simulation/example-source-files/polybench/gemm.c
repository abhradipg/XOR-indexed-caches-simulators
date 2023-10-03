
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define NI 20
#   define NJ 25
#   define NK 30
#  endif

#  ifdef SMALL_DATASET
#   define NI 60
#   define NJ 70
#   define NK 80
#  endif

#  ifdef MEDIUM_DATASET
#   define NI 200
#   define NJ 220
#   define NK 240
#  endif

#  ifdef LARGE_DATASET
#   define NI 1000
#   define NJ 1100
#   define NK 1200
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NI 2000
#   define NJ 2300
#   define NK 2600
#  endif

void kernel_gemm() {
  double C[NI][NJ];
  double A[NI][NK];
  double B[NK][NJ];
  double alpha = 1.5;
  double beta = 1.2;  
  int i, j, k;

#pragma scop
  for (i = 0; i < NI; i++) {
    for (j = 0; j < NJ; j++)
      C[i][j] *= beta;
    for (k = 0; k < NK; k++) {
      for (j = 0; j < NJ; j++)
        C[i][j] += alpha * A[i][k] * B[k][j];
    }
  }
#pragma endscop
  
}
