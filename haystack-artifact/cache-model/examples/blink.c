int main() {
  int t, i, j;
  int N1, N2;
  float A[N1][N2];

#pragma scop
  for (t = 0; t < 4; ++t) {
    for (i = 0; i < N1; i++)
      for (j = 0; j < N2; j++)
S0:     A[i][j] = 0.0;
    for (i = 0; i < N1; i++)
      for (j = 0; j < N2; j++)
S1:     A[i][j] = 1.0;
  }
#pragma endscop
}
