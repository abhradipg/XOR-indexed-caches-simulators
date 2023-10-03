#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <cassert>
#include <isl/cpp2.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

// computation options
//#define RASTERIZATION 1
//#define EQUALIZATION 1
//#define COMPUTE_BOUNDS 1
//#define COMPUTE_CONFLICTS 1
#define DIMENSION_COUNTING 1
#define TIMERS 1
//#define PREFETCHING 1
//#define ENUMERATE_POINTS 1

// flags to control the verification steps
#ifndef NDEBUG
#define VERIFY_RESULT 1
#endif

// struct defining the machine properties
struct machine_model {
  long CacheLineSize;
  std::vector<long> CacheSizes;
};

// struct holding the term information
struct term {
  isl::val Coefficient;
  // the exponents
  std::vector<int> Parameters;
  std::vector<int> Variables;
  std::vector<int> Divisors;
  // the term
  isl::qpolynomial Polynomial;
};

// struct holding the piece information
struct piece {
  isl::set Domain;
  long Size;
  // polynomial
  isl::qpolynomial Polynomial;
  // terms of the polynomial
  std::vector<term> Terms;
  // affine form
  isl::pw_aff Expression;
};

// statistics about the counting method
struct methods {
  long Barvinok;
  long Bernstein;
  long Enumerate;
};

// statistics about the splits introduced
struct splits {
  long Barvinok;
  long Equalize;
  long Raster;
  long Enumerate;
};

// struct holding prefetch info
struct prefetch_info {
  bool UnitStride;
  std::vector<int> PrefetchDepth;
  std::vector<int> PrefetchStreams;
  std::vector<bool> Prefetched;
};

// struct holding the cache misses and compute statistics
struct misses {
  long Total;
  long Counted;
  long CompulsoryMisses;
  std::vector<long> CapacityMisses;
  prefetch_info PrefetchInfo;
  methods Methods;
  splits Splits;
  std::map<std::vector<int>, int> Affinity;
};

// define named types
typedef std::pair<std::string, int> NamedInt;
typedef std::pair<std::string, long> NamedLong;
typedef std::pair<std::string, std::vector<long>> NamedVector;
typedef std::pair<std::string, misses> NamedMisses;

// compute the integer power
inline long compute_power(long base, int exponent) {
  assert(exponent >= 0);
  long Result = 1;
  while (exponent) {
    if (exponent & 1)
      Result *= base;
    base *= base;
    exponent /= 2;
  }
  return Result;
};

#endif