
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define TSTEPS 20
#   define N 20
#  endif

#  ifdef SMALL_DATASET
#   define TSTEPS 40
#   define N 60
#  endif

#  ifdef MEDIUM_DATASET
#   define TSTEPS 100
#   define N 200
#  endif

#  ifdef LARGE_DATASET
#   define TSTEPS 500
#   define N 1000
#  endif

#  ifdef EXTRALARGE_DATASET
#   define TSTEPS 1000
#   define N 2000
#  endif

void kernel_adi() {
  double u[N][N];
  double v[N][N];
  double p[N][N];
  double q[N][N];
  double DX, DY, DT;
  double B1, B2;
  double mul1, mul2;
  double a, b, c, d, e, f;
  int t, i, j;

#pragma scop

  DX = 1.0/(double)N;
  DY = 1.0/(double)N;
  DT = 1.0/(double)TSTEPS;
  B1 = 2.0;
  B2 = 1.0;
  mul1 = B1 * DT / (DX * DX);
  mul2 = B2 * DT / (DY * DY);

  a = -mul1 / 2.0;
  b = 1.0 +mul1;
  c = a;
  d = -mul2 / 2.0;
  e = 1.0 +mul2;
  f = d;

 for (t=1; t<=TSTEPS; t++) {
    for (i=1; i<N-1; i++) {
      v[0][i] = 1.0;
      p[i][0] = 0.0;
      q[i][0] = v[0][i];
      for (j=1; j<N-1; j++) {
        p[i][j] = -c / (a*p[i][j-1]+b);
        q[i][j] = (-d*u[j][i-1]+(1.0 +2.0*d)*u[j][i] - f*u[j][i+1]-a*q[i][j-1])/(a*p[i][j-1]+b);
      }
      v[N-1][i] = 1.0;
      for (j=N-2; j>=1; j--) {
        v[j][i] = p[i][j] * v[j+1][i] + q[i][j];
      }
    }
    for (i=1; i<N-1; i++) {
      u[i][0] = 1.0;
      p[i][0] = 0.0;
      q[i][0] = u[i][0];
      for (j=1; j<N-1; j++) {
        p[i][j] = -f / (d*p[i][j-1]+e);
        q[i][j] = (-a*v[i-1][j]+(1.0 +2.0*a)*v[i][j] - c*v[i+1][j]-d*q[i][j-1])/(d*p[i][j-1]+e);
      }
      u[i][N-1] = 1.0;
      for (j=N-2; j>=1; j--) {
        u[i][j] = p[i][j] * u[i][j+1] + q[i][j];
      }
    }
  }
#pragma endscop
  
}
