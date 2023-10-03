#define DIM1 100
#define DIM2 100

int main() {
  double sum;
  double arr[DIM1][DIM2];
  for (int i = 0; i < DIM1; i++) {
    for (int j = 0; j < DIM2; j++) {
      arr[i][j] = i + j;
    }
  }

#pragma scop
  for (int i = 0; i < DIM1; i++) {
    for (int j = 0; j < DIM2; j++) {
      sum += arr[i][j] + arr[j][i];
    }
  }

#pragma endscop
  return 0;
}
