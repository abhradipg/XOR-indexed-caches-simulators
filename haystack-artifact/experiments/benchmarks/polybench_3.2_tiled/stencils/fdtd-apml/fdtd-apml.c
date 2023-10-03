/**
 * fdtd-apml.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 256x256x256. */
#include "fdtd-apml.h"


/* Array initialization. */
static
void init_array (int cz,
		 int cxm,
		 int cym,
		 DATA_TYPE *mui,
		 DATA_TYPE *ch,
		 DATA_TYPE POLYBENCH_2D(Ax,CZ+1,CYM+1,cz+1,cym+1),
		 DATA_TYPE POLYBENCH_2D(Ry,CZ+1,CYM+1,cz+1,cym+1),
		 DATA_TYPE POLYBENCH_3D(Ex,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_3D(Ey,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_3D(Hz,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_1D(czm,CZ+1,cz+1),
		 DATA_TYPE POLYBENCH_1D(czp,CZ+1,cz+1),
		 DATA_TYPE POLYBENCH_1D(cxmh,CXM+1,cxm+1),
		 DATA_TYPE POLYBENCH_1D(cxph,CXM+1,cxm+1),
		 DATA_TYPE POLYBENCH_1D(cymh,CYM+1,cym+1),
		 DATA_TYPE POLYBENCH_1D(cyph,CYM+1,cym+1))
{
  int i, j, k;
  *mui = 2341;
  *ch = 42;
  for (i = 0; i <= cz; i++)
    {
      czm[i] = ((DATA_TYPE) i + 1) / cxm;
      czp[i] = ((DATA_TYPE) i + 2) / cxm;
    }
  for (i = 0; i <= cxm; i++)
    {
      cxmh[i] = ((DATA_TYPE) i + 3) / cxm;
      cxph[i] = ((DATA_TYPE) i + 4) / cxm;
    }
  for (i = 0; i <= cym; i++)
    {
      cymh[i] = ((DATA_TYPE) i + 5) / cxm;
      cyph[i] = ((DATA_TYPE) i + 6) / cxm;
    }

  for (i = 0; i <= cz; i++)
    for (j = 0; j <= cym; j++)
      {
	Ry[i][j] = ((DATA_TYPE) i*(j+1) + 10) / cym;
	Ax[i][j] = ((DATA_TYPE) i*(j+2) + 11) / cym;
	for (k = 0; k <= cxm; k++)
	  {
	    Ex[i][j][k] = ((DATA_TYPE) i*(j+3) + k + 1) / cxm;
	    Ey[i][j][k] = ((DATA_TYPE) i*(j+4) + k + 2) / cym;
	    Hz[i][j][k] = ((DATA_TYPE) i*(j+5) + k + 3) / cz;
	  }
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int cz,
		 int cxm,
		 int cym,
		 DATA_TYPE POLYBENCH_3D(Bza,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_3D(Ex,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_3D(Ey,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		 DATA_TYPE POLYBENCH_3D(Hz,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1))
{
  int i, j, k;

  for (i = 0; i <= cz; i++)
    for (j = 0; j <= cym; j++)
      for (k = 0; k <= cxm; k++) {
	fprintf(stderr, DATA_PRINTF_MODIFIER, Bza[i][j][k]);
	fprintf(stderr, DATA_PRINTF_MODIFIER, Ex[i][j][k]);
	fprintf(stderr, DATA_PRINTF_MODIFIER, Ey[i][j][k]);
	fprintf(stderr, DATA_PRINTF_MODIFIER, Hz[i][j][k]);
	if ((i * cxm + j) % 20 == 0) fprintf(stderr, "\n");
      }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_fdtd_apml(int cz,
		      int cxm,
		      int cym,
		      DATA_TYPE mui,
		      DATA_TYPE ch,
		      DATA_TYPE POLYBENCH_2D(Ax,CZ+1,CYM+1,cz+1,cym+1),
		      DATA_TYPE POLYBENCH_2D(Ry,CZ+1,CYM+1,cz+1,cym+1),
		      DATA_TYPE POLYBENCH_2D(clf,CYM+1,CXM+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_2D(tmp,CYM+1,CXM+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_3D(Bza,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_3D(Ex,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_3D(Ey,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_3D(Hz,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1),
		      DATA_TYPE POLYBENCH_1D(czm,CZ+1,cz+1),
		      DATA_TYPE POLYBENCH_1D(czp,CZ+1,cz+1),
		      DATA_TYPE POLYBENCH_1D(cxmh,CXM+1,cxm+1),
		      DATA_TYPE POLYBENCH_1D(cxph,CXM+1,cxm+1),
		      DATA_TYPE POLYBENCH_1D(cymh,CYM+1,cym+1),
		      DATA_TYPE POLYBENCH_1D(cyph,CYM+1,cym+1))
{
  int iz, iy, ix;

#pragma scop
  /* ppcg generated CPU code */
  
  {
    for (int c0 = 0; c0 <= 511; c0 += 32)
      for (int c1 = 0; c1 <= 511; c1 += 32)
        for (int c2 = 0; c2 <= 511; c2 += 32)
          for (int c3 = 0; c3 <= 31; c3 += 1)
            for (int c4 = 0; c4 <= 31; c4 += 1)
              for (int c5 = 0; c5 <= 31; c5 += 1) {
                clf[c0 + c3][c2 + c5] = (((Ex[c0 + c3][c2 + c5][c1 + c4] - Ex[c0 + c3][c2 + c5 + 1][c1 + c4]) + Ey[c0 + c3][c2 + c5][c1 + c4 + 1]) - Ey[c0 + c3][c2 + c5][c1 + c4]);
                tmp[c0 + c3][c2 + c5] = (((cymh[c2 + c5] / cyph[c2 + c5]) * Bza[c0 + c3][c2 + c5][c1 + c4]) - ((ch / cyph[c2 + c5]) * clf[c0 + c3][c2 + c5]));
                Hz[c0 + c3][c2 + c5][c1 + c4] = ((((cxmh[c1 + c4] / cxph[c1 + c4]) * Hz[c0 + c3][c2 + c5][c1 + c4]) + (((mui * czp[c0 + c3]) / cxph[c1 + c4]) * tmp[c0 + c3][c2 + c5])) - (((mui * czm[c0 + c3]) / cxph[c1 + c4]) * Bza[c0 + c3][c2 + c5][c1 + c4]));
                Bza[c0 + c3][c2 + c5][c1 + c4] = tmp[c0 + c3][c2 + c5];
                if (c0 == 0 && c3 == 0) {
                  clf[c1 + c4][c2 + c5] = (((Ex[c1 + c4][c2 + c5][512] - Ex[c1 + c4][c2 + c5 + 1][512]) + Ry[c1 + c4][c2 + c5]) - Ey[c1 + c4][c2 + c5][512]);
                  tmp[c1 + c4][c2 + c5] = (((cymh[c2 + c5] / cyph[c2 + c5]) * Bza[c1 + c4][c2 + c5][512]) - ((ch / cyph[c2 + c5]) * clf[c1 + c4][c2 + c5]));
                  Hz[c1 + c4][c2 + c5][512] = ((((cxmh[512] / cxph[512]) * Hz[c1 + c4][c2 + c5][512]) + (((mui * czp[c1 + c4]) / cxph[512]) * tmp[c1 + c4][c2 + c5])) - (((mui * czm[c1 + c4]) / cxph[512]) * Bza[c1 + c4][c2 + c5][512]));
                  Bza[c1 + c4][c2 + c5][512] = tmp[c1 + c4][c2 + c5];
                }
                clf[c0 + c3][c2 + c5] = (((Ex[c0 + c3][512][c1 + c4] - Ax[c0 + c3][c1 + c4]) + Ey[c0 + c3][512][c1 + c4 + 1]) - Ey[c0 + c3][512][c1 + c4]);
                tmp[c0 + c3][c2 + c5] = (((cymh[512] / cyph[c2 + c5]) * Bza[c0 + c3][c2 + c5][c1 + c4]) - ((ch / cyph[c2 + c5]) * clf[c0 + c3][c2 + c5]));
                Hz[c0 + c3][512][c1 + c4] = ((((cxmh[c1 + c4] / cxph[c1 + c4]) * Hz[c0 + c3][512][c1 + c4]) + (((mui * czp[c0 + c3]) / cxph[c1 + c4]) * tmp[c0 + c3][c2 + c5])) - (((mui * czm[c0 + c3]) / cxph[c1 + c4]) * Bza[c0 + c3][512][c1 + c4]));
                Bza[c0 + c3][512][c1 + c4] = tmp[c0 + c3][c2 + c5];
              }
    for (int c0 = 0; c0 <= 511; c0 += 32)
      for (int c1 = 0; c1 <= 511; c1 += 32)
        for (int c2 = 0; c2 <= 31; c2 += 1)
          for (int c3 = 0; c3 <= 31; c3 += 1) {
            clf[c0 + c2][c1 + c3] = (((Ex[c0 + c2][512][512] - Ax[c0 + c2][512]) + Ry[c0 + c2][512]) - Ey[c0 + c2][512][512]);
            tmp[c0 + c2][c1 + c3] = (((cymh[512] / cyph[512]) * Bza[c0 + c2][512][512]) - ((ch / cyph[512]) * clf[c0 + c2][c1 + c3]));
            Hz[c0 + c2][512][512] = ((((cxmh[512] / cxph[512]) * Hz[c0 + c2][512][512]) + (((mui * czp[c0 + c2]) / cxph[512]) * tmp[c0 + c2][c1 + c3])) - (((mui * czm[c0 + c2]) / cxph[512]) * Bza[c0 + c2][512][512]));
            Bza[c0 + c2][512][512] = tmp[c0 + c2][c1 + c3];
          }
  }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int cz = CZ;
  int cym = CYM;
  int cxm = CXM;

  /* Variable declaration/allocation. */
  DATA_TYPE mui;
  DATA_TYPE ch;
  POLYBENCH_2D_ARRAY_DECL(Ax,DATA_TYPE,CZ+1,CYM+1,cz+1,cym+1);
  POLYBENCH_2D_ARRAY_DECL(Ry,DATA_TYPE,CZ+1,CYM+1,cz+1,cym+1);
  POLYBENCH_2D_ARRAY_DECL(clf,DATA_TYPE,CYM+1,CXM+1,cym+1,cxm+1);
  POLYBENCH_2D_ARRAY_DECL(tmp,DATA_TYPE,CYM+1,CXM+1,cym+1,cxm+1);
  POLYBENCH_3D_ARRAY_DECL(Bza,DATA_TYPE,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1);
  POLYBENCH_3D_ARRAY_DECL(Ex,DATA_TYPE,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1);
  POLYBENCH_3D_ARRAY_DECL(Ey,DATA_TYPE,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1);
  POLYBENCH_3D_ARRAY_DECL(Hz,DATA_TYPE,CZ+1,CYM+1,CXM+1,cz+1,cym+1,cxm+1);
  POLYBENCH_1D_ARRAY_DECL(czm,DATA_TYPE,CZ+1,cz+1);
  POLYBENCH_1D_ARRAY_DECL(czp,DATA_TYPE,CZ+1,cz+1);
  POLYBENCH_1D_ARRAY_DECL(cxmh,DATA_TYPE,CXM+1,cxm+1);
  POLYBENCH_1D_ARRAY_DECL(cxph,DATA_TYPE,CXM+1,cxm+1);
  POLYBENCH_1D_ARRAY_DECL(cymh,DATA_TYPE,CYM+1,cym+1);
  POLYBENCH_1D_ARRAY_DECL(cyph,DATA_TYPE,CYM+1,cym+1);

  /* Initialize array(s). */
  init_array (cz, cxm, cym, &mui, &ch,
  	      POLYBENCH_ARRAY(Ax),
  	      POLYBENCH_ARRAY(Ry),
  	      POLYBENCH_ARRAY(Ex),
  	      POLYBENCH_ARRAY(Ey),
  	      POLYBENCH_ARRAY(Hz),
  	      POLYBENCH_ARRAY(czm),
  	      POLYBENCH_ARRAY(czp),
  	      POLYBENCH_ARRAY(cxmh),
  	      POLYBENCH_ARRAY(cxph),
  	      POLYBENCH_ARRAY(cymh),
  	      POLYBENCH_ARRAY(cyph));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_fdtd_apml (cz, cxm, cym, mui, ch,
  		    POLYBENCH_ARRAY(Ax),
  		    POLYBENCH_ARRAY(Ry),
  		    POLYBENCH_ARRAY(clf),
  		    POLYBENCH_ARRAY(tmp),
  		    POLYBENCH_ARRAY(Bza),
  		    POLYBENCH_ARRAY(Ex),
  		    POLYBENCH_ARRAY(Ey),
  		    POLYBENCH_ARRAY(Hz),
  		    POLYBENCH_ARRAY(czm),
  		    POLYBENCH_ARRAY(czp),
  		    POLYBENCH_ARRAY(cxmh),
  		    POLYBENCH_ARRAY(cxph),
  		    POLYBENCH_ARRAY(cymh),
  		    POLYBENCH_ARRAY(cyph));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(cz, cxm, cym,
  				    POLYBENCH_ARRAY(Bza),
  				    POLYBENCH_ARRAY(Ex),
  				    POLYBENCH_ARRAY(Ey),
  				    POLYBENCH_ARRAY(Hz)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(Ax);
  POLYBENCH_FREE_ARRAY(Ry);
  POLYBENCH_FREE_ARRAY(clf);
  POLYBENCH_FREE_ARRAY(tmp);
  POLYBENCH_FREE_ARRAY(Bza);
  POLYBENCH_FREE_ARRAY(Ex);
  POLYBENCH_FREE_ARRAY(Ey);
  POLYBENCH_FREE_ARRAY(Hz);
  POLYBENCH_FREE_ARRAY(czm);
  POLYBENCH_FREE_ARRAY(czp);
  POLYBENCH_FREE_ARRAY(cxmh);
  POLYBENCH_FREE_ARRAY(cxph);
  POLYBENCH_FREE_ARRAY(cymh);
  POLYBENCH_FREE_ARRAY(cyph);

  return 0;
}

