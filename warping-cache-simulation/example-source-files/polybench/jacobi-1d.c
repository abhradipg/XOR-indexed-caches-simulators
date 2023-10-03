

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TSTEPS 20
#   define N 30
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

void kernel_jacobi_1d() {
  double A[N];
  double B[N];
  int t, i;

#pragma scop
  for (t = 0; t < TSTEPS; t++) {
    for (i = 1; i < N - 1; i++)
      B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
    for (i = 1; i < N - 1; i++)
      A[i] = 0.33333 * (B[i-1] + B[i] + B[i + 1]);
  }
#pragma endscop

}
