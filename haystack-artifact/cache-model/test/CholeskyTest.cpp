#include "gtest/gtest.h"

#include <isl/options.h>

#include "../src/CacheModel.h"
#include "CacheEmulator.h"

// test setup
const int ElementSize = 4;
const int CacheLineSize = 16 / ElementSize;
const int CacheSize = 1024 / ElementSize;

// execute cholesky with the emulator
void emulateCholesky(int N, int CacheLineSize, CacheEmulator &Emulator) {
  int TimeStamp = 0;
  // define the index to cachline conversion
  int Stride = (N + CacheLineSize - 1) / CacheLineSize;
  auto CL = [&](int i, int j) { return i * Stride + j / CacheLineSize; };
  // run cholesky
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < i; j++) {
      for (int k = 0; k < j; k++) {
        // A[i][j] -= A[i][k] * A[j][k];
        Emulator.accessMemory("S0", TimeStamp, {CL(i, j), CL(i, k), CL(j, k), CL(i, j)});
      }
      // A[i][j] /= A[j][j];
      Emulator.accessMemory("S1", TimeStamp, {CL(i, j), CL(j, j), CL(i, j)});
    }

    for (int k = 0; k < i; k++) {
      // A[i][i] -= A[i][k] * A[i][k];
      Emulator.accessMemory("S2", TimeStamp, {CL(i, i), CL(i, k), CL(i, k), CL(i, i)});
    }
    // A[i][i] = SQRT_FUN(A[i][i]);
    Emulator.accessMemory("S3", TimeStamp, {CL(i, i), CL(i, i)});
  }
}

class CholeskyTest : public ::testing::Test {
protected:
  CholeskyTest() {
    Context_ = isl_ctx_alloc_with_pet_options();
    isl_options_set_on_error(Context_, ISL_ON_ERROR_ABORT);

    Base_ = new CacheModel(Context_, {CacheLineSize * ElementSize, {CacheSize * ElementSize}});
    Base_->compileProgram("./cholesky.c");
  }

  virtual ~CholeskyTest() {
    delete Base_;
    isl_ctx_free(Context_);
  }

  virtual void SetUp() {
    // get fresh copy for every test
    Model_ = new CacheModel(*Base_);
  }

  virtual void TearDown() { delete Model_; }

  isl_ctx *Context_;
  CacheModel *Base_;
  CacheModel *Model_;
};

TEST_F(CholeskyTest, CapacityMissesEven) {
  // define the setup
  int N = 32;
  int CacheLines = N * ((N + CacheLineSize - 1) / CacheLineSize);
  std::vector<NamedLong> Parameters = {std::make_pair(std::string("N"), N)};
  // emulate the stack distances
  CacheEmulator Emulator(CacheLines, CacheSize / CacheLineSize);
  emulateCholesky(N, CacheLineSize, Emulator);
  auto ExpectedCapacityMisses = Emulator.getCapacityMisses();
  // compute the stack distances
  Model_->initModel(Parameters);
  //
  std::map<std::string, std::vector<long>> ComputedCapacityMisses;
  for (auto ComputedCapacityMiss : Model_->countCacheMisses()) {
    std::string Statement = ComputedCapacityMiss.first;
    auto length = Statement.find_first_of("(");
    Statement = Statement.substr(0, length);
    ComputedCapacityMisses[Statement].push_back(ComputedCapacityMiss.second.CapacityMisses[0]);
  }

  // print computed and expected stack distances
  for (auto ComputedCapacityMiss : ComputedCapacityMisses) {
    printf("Computed %s -> ", ComputedCapacityMiss.first.c_str());
    for (auto Distance : ComputedCapacityMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }
  for (auto ExpectedCapacityMiss : ExpectedCapacityMisses) {
    printf("Expected %s -> ", ExpectedCapacityMiss.first.c_str());
    for (auto Distance : ExpectedCapacityMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }

  // make sure the sizes agree
  ASSERT_EQ(ExpectedCapacityMisses.size(), ComputedCapacityMisses.size());

  // compare the stack distances for all statements
  for (auto ComputedCapacityMiss : ComputedCapacityMisses) {
    auto ExpectedCapacityMiss = ExpectedCapacityMisses[ComputedCapacityMiss.first];
    ASSERT_EQ(ExpectedCapacityMiss.size(), ComputedCapacityMiss.second.size());

    for (int i = 0; i < ComputedCapacityMiss.second.size(); ++i)
      EXPECT_EQ(ExpectedCapacityMiss[i], ComputedCapacityMiss.second[i]);
  }
}

TEST_F(CholeskyTest, CompulsoryMissesEven) {
  // define the setup
  int N = 32;
  int CacheLines = N * ((N + CacheLineSize - 1) / CacheLineSize);
  std::vector<NamedLong> Parameters = {std::make_pair(std::string("N"), N)};
  // emulate the stack distances
  CacheEmulator Emulator(CacheLines, CacheSize / CacheLineSize);
  emulateCholesky(N, CacheLineSize, Emulator);
  auto ExpectedCompulsoryMisses = Emulator.getCompulsoryMisses();
  // compute the stack distances
  Model_->initModel(Parameters);
  //
  std::map<std::string, std::vector<long>> ComputedCompulsoryMisses;
  for (auto ComputedCompulsoryMiss : Model_->countCacheMisses()) {
    std::string Statement = ComputedCompulsoryMiss.first;
    auto length = Statement.find_first_of("(");
    Statement = Statement.substr(0, length);
    ComputedCompulsoryMisses[Statement].push_back(ComputedCompulsoryMiss.second.CompulsoryMisses);
  }

  // print computed and expected stack distances
  for (auto ComputedCompulsoryMiss : ComputedCompulsoryMisses) {
    printf("Computed %s -> ", ComputedCompulsoryMiss.first.c_str());
    for (auto Distance : ComputedCompulsoryMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }
  for (auto ExpectedCompulsoryMiss : ExpectedCompulsoryMisses) {
    printf("Expected %s -> ", ExpectedCompulsoryMiss.first.c_str());
    for (auto Distance : ExpectedCompulsoryMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }

  // make sure the sizes agree
  ASSERT_EQ(ExpectedCompulsoryMisses.size(), ComputedCompulsoryMisses.size());

  // compare the stack distances for all statements
  for (auto ComputedCompulsoryMiss : ComputedCompulsoryMisses) {
    auto ExpectedCompulsoryMiss = ExpectedCompulsoryMisses[ComputedCompulsoryMiss.first];
    ASSERT_EQ(ExpectedCompulsoryMiss.size(), ComputedCompulsoryMiss.second.size());

    for (int i = 0; i < ComputedCompulsoryMiss.second.size(); ++i)
      EXPECT_EQ(ExpectedCompulsoryMiss[i], ComputedCompulsoryMiss.second[i]);
  }
}

TEST_F(CholeskyTest, CapacityMissesOdd) {
  // define the setup
  int N = 33;
  int CacheLines = N * ((N + CacheLineSize - 1) / CacheLineSize);
  std::vector<NamedLong> Parameters = {std::make_pair(std::string("N"), N)};
  // emulate the stack distances
  CacheEmulator Emulator(CacheLines, CacheSize / CacheLineSize);
  emulateCholesky(N, CacheLineSize, Emulator);
  auto ExpectedCapacityMisses = Emulator.getCapacityMisses();
  // compute the stack distances
  Model_->initModel(Parameters);
  //
  std::map<std::string, std::vector<long>> ComputedCapacityMisses;
  for (auto ComputedCapacityMiss : Model_->countCacheMisses()) {
    std::string Statement = ComputedCapacityMiss.first;
    auto length = Statement.find_first_of("(");
    Statement = Statement.substr(0, length);
    ComputedCapacityMisses[Statement].push_back(ComputedCapacityMiss.second.CapacityMisses[0]);
  }

  // print computed and expected stack distances
  for (auto ComputedCapacityMiss : ComputedCapacityMisses) {
    printf("Computed %s -> ", ComputedCapacityMiss.first.c_str());
    for (auto Distance : ComputedCapacityMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }
  for (auto ExpectedCapacityMiss : ExpectedCapacityMisses) {
    printf("Expected %s -> ", ExpectedCapacityMiss.first.c_str());
    for (auto Distance : ExpectedCapacityMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }

  // make sure the sizes agree
  ASSERT_EQ(ExpectedCapacityMisses.size(), ComputedCapacityMisses.size());

  // compare the stack distances for all statements
  for (auto ComputedCapacityMiss : ComputedCapacityMisses) {
    auto ExpectedCapacityMiss = ExpectedCapacityMisses[ComputedCapacityMiss.first];
    ASSERT_EQ(ExpectedCapacityMiss.size(), ComputedCapacityMiss.second.size());

    for (int i = 0; i < ComputedCapacityMiss.second.size(); ++i)
      EXPECT_EQ(ExpectedCapacityMiss[i], ComputedCapacityMiss.second[i]);
  }
}

TEST_F(CholeskyTest, CompulsoryMissesOdd) {
  // define the setup
  int N = 33;
  int CacheLines = N * ((N + CacheLineSize - 1) / CacheLineSize);
  std::vector<NamedLong> Parameters = {std::make_pair(std::string("N"), N)};
  // emulate the stack distances
  CacheEmulator Emulator(CacheLines, CacheSize / CacheLineSize);
  emulateCholesky(N, CacheLineSize, Emulator);
  auto ExpectedCompulsoryMisses = Emulator.getCompulsoryMisses();
  // compute the stack distances
  Model_->initModel(Parameters);
  //
  std::map<std::string, std::vector<long>> ComputedCompulsoryMisses;
  for (auto ComputedCompulsoryMiss : Model_->countCacheMisses()) {
    std::string Statement = ComputedCompulsoryMiss.first;
    auto length = Statement.find_first_of("(");
    Statement = Statement.substr(0, length);
    ComputedCompulsoryMisses[Statement].push_back(ComputedCompulsoryMiss.second.CompulsoryMisses);
  }

  // print computed and expected stack distances
  for (auto ComputedCompulsoryMiss : ComputedCompulsoryMisses) {
    printf("Computed %s -> ", ComputedCompulsoryMiss.first.c_str());
    for (auto Distance : ComputedCompulsoryMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }
  for (auto ExpectedCompulsoryMiss : ExpectedCompulsoryMisses) {
    printf("Expected %s -> ", ExpectedCompulsoryMiss.first.c_str());
    for (auto Distance : ExpectedCompulsoryMiss.second)
      printf("%ld ", Distance);
    printf("\n");
  }

  // make sure the sizes agree
  ASSERT_EQ(ExpectedCompulsoryMisses.size(), ComputedCompulsoryMisses.size());

  // compare the stack distances for all statements
  for (auto ComputedCompulsoryMiss : ComputedCompulsoryMisses) {
    auto ExpectedCompulsoryMiss = ExpectedCompulsoryMisses[ComputedCompulsoryMiss.first];
    ASSERT_EQ(ExpectedCompulsoryMiss.size(), ComputedCompulsoryMiss.second.size());

    for (int i = 0; i < ComputedCompulsoryMiss.second.size(); ++i)
      EXPECT_EQ(ExpectedCompulsoryMiss[i], ComputedCompulsoryMiss.second[i]);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}