
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define NI 16
#   define NJ 18
#   define NK 20
#   define NL 22
#   define NM 24
#  endif

#  ifdef SMALL_DATASET
#   define NI 40
#   define NJ 50
#   define NK 60
#   define NL 70
#   define NM 80
#  endif

#  ifdef MEDIUM_DATASET
#   define NI 180
#   define NJ 190
#   define NK 200
#   define NL 210
#   define NM 220
#  endif

#  ifdef LARGE_DATASET
#   define NI 800
#   define NJ 900
#   define NK 1000
#   define NL 1100
#   define NM 1200
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NI 1600
#   define NJ 1800
#   define NK 2000
#   define NL 2200
#   define NM 2400
#  endif

void kernel_3mm() {
  double E[NI][NJ];
  double A[NI][NK];
  double B[NK][NJ];
  double F[NJ][NL];
  double C[NJ][NM];
  double D[NM][NL];
  double G[NI][NL];
  int i, j, k;

#pragma scop
  for (i = 0; i < NI; i++)
    for (j = 0; j < NJ; j++) {
      E[i][j] = 0.0;
      for (k = 0; k < NK; ++k)
        E[i][j] += A[i][k] * B[k][j];
    }

  for (i = 0; i < NJ; i++)
    for (j = 0; j < NL; j++) {
      F[i][j] = 0.0;
      for (k = 0; k < NM; ++k)
        F[i][j] += C[i][k] * D[k][j];
    }

  for (i = 0; i < NI; i++)
    for (j = 0; j < NL; j++) {
      G[i][j] = 0.0;
      for (k = 0; k < NJ; ++k)
        G[i][j] += E[i][k] * F[k][j];
    }
#pragma endscop

}
