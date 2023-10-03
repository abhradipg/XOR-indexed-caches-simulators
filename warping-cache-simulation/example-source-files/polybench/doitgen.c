
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define NQ 8
#   define NR 10
#   define NP 12
#  endif

#  ifdef SMALL_DATASET
#   define NQ 20
#   define NR 25
#   define NP 30
#  endif

#  ifdef MEDIUM_DATASET
#   define NQ 40
#   define NR 50
#   define NP 60
#  endif

#  ifdef LARGE_DATASET
#   define NQ 140
#   define NR 150
#   define NP 160
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NQ 220
#   define NR 250
#   define NP 270
#  endif

void kernel_doitgen() {
  double A[NR][NQ][NP];
  double C4[NP][NP];
  double sum[NP];
  int r, q, p, s;

#pragma scop
    for (r = 0; r < NR; r++)
      for (q = 0; q < NQ; q++) {
        for (p = 0; p < NP; p++) {
          sum[p] = 0.0;
          for (s = 0; s < NP; s++)
            sum[p] += A[r][q][s] * C4[s][p];
        }
        for (p = 0; p < NP; p++)
          A[r][q][p] = sum[p];
      }
#pragma endscop

}