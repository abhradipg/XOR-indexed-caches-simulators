
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

#  ifdef MINI_DATASET
#   define W 64
#   define H 64
#  endif

#  ifdef SMALL_DATASET
#   define W 192
#   define H 128
#  endif

#  ifdef MEDIUM_DATASET
#   define W 720
#   define H 480
#  endif

#  ifdef LARGE_DATASET
#   define W 4096
#   define H 2160
#  endif

#  ifdef EXTRALARGE_DATASET
#   define W 7680
#   define H 4320
#  endif

void kernel_deriche() {
  double imgIn[W][H];
  double imgOut[W][H];
  double y1[W][H];
  double y2[W][H];
  double alpha = 0.25;
  double xm1, tm1, ym1, ym2;
  double xp1, xp2;
  double tp1, tp2;
  double yp1, yp2;
  double k;
  double a1, a2, a3, a4, a5, a6, a7, a8;
  double b1, b2, c1, c2;
  int i,j;

#pragma scop
/*
    k = (1.0f -expf(-alpha))*(1.0f -expf(-alpha))/(1.0f +2.0f*alpha*expf(-alpha)-expf(2.0f*alpha));
    a1 = a5 = k;
    a2 = a6 = k*expf(-alpha)*(alpha-1.0f);
    a3 = a7 = k*expf(-alpha)*(alpha+1.0f);
    a4 = a8 = -k*expf(-2.0f*alpha);
    b1 = powf(2.0f,-alpha);
    b2 = -expf(-2.0f*alpha);
    c1 = c2 = 1;
*/

    for (i=0; i<W; i++) {
      ym1 = 0.0f;
      ym2 = 0.0f;
      xm1 = 0.0f;
      for (j=0; j<H; j++) {
        y1[i][j] = a1*imgIn[i][j] + a2*xm1 + b1*ym1 + b2*ym2;
        xm1 = imgIn[i][j];
        ym2 = ym1;
        ym1 = y1[i][j];
      }
    }

    for (i=0; i<W; i++) {
      yp1 = 0.0f;
      yp2 = 0.0f;
      xp1 = 0.0f;
      xp2 = 0.0f;
      for (j=H-1; j>=0; j--) {
        y2[i][j] = a3*xp1 + a4*xp2 + b1*yp1 + b2*yp2;
        xp2 = xp1;
        xp1 = imgIn[i][j];
        yp2 = yp1;
        yp1 = y2[i][j];
      }
    }

    for (i=0; i<W; i++)
      for (j=0; j<H; j++) {
        imgOut[i][j] = c1 * (y1[i][j] + y2[i][j]);
      }

    for (j=0; j<H; j++) {
      tm1 = 0.0f;
      ym1 = 0.0f;
      ym2 = 0.0f;
      for (i=0; i<W; i++) {
        y1[i][j] = a5*imgOut[i][j] + a6*tm1 + b1*ym1 + b2*ym2;
        tm1 = imgOut[i][j];
        ym2 = ym1;
        ym1 = y1 [i][j];
      }
    }

    for (j=0; j<H; j++) {
      tp1 = 0.0f;
      tp2 = 0.0f;
      yp1 = 0.0f;
      yp2 = 0.0f;
      for (i=W-1; i>=0; i--) {
        y2[i][j] = a7*tp1 + a8*tp2 + b1*yp1 + b2*yp2;
        tp2 = tp1;
        tp1 = imgOut[i][j];
        yp2 = yp1;
        yp1 = y2[i][j];
      }
    }

    for (i=0; i<W; i++)
      for (j=0; j<H; j++)
        imgOut[i][j] = c2*(y1[i][j] + y2[i][j]);

#pragma endscop

}
