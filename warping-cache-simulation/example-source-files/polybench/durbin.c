

# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define N 40
#  endif

#  ifdef SMALL_DATASET
#   define N 120
#  endif

#  ifdef MEDIUM_DATASET
#   define N 400
#  endif

#  ifdef LARGE_DATASET
#   define N 2000
#  endif

#  ifdef EXTRALARGE_DATASET
#   define N 4000
#  endif

void kernel_durbin() {
  double r[N];
  double y[N];
  double z[N];
  double alpha;
  double beta;
  double sum;
  int i,k;

#pragma scop
  y[0] = -r[0];
  beta = 1.0;
  alpha = -r[0];

  for (k = 1; k < N; k++) {
    beta = (1-alpha*alpha)*beta;
    sum = 0.0;
    for (i=0; i<k; i++) {
      sum += r[k-i-1]*y[i];
    }
    alpha = - (r[k] + sum)/beta;
    for (i=0; i<k; i++) {
      z[i] = y[i] + alpha*y[k-i-1];
    }
    for (i=0; i<k; i++) {
      y[i] = z[i];
    }
    y[k] = alpha;
  }
#pragma endscop

}