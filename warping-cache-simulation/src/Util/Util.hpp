#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <isl/cpp.h>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Util/GlobalVars.hpp"

namespace Util {

[[nodiscard]] isl::aff
CalculateAffineAddress(size_t startingAddress, size_t elementSize,
                       const std::vector<size_t> &arrayDims,
                       const std::vector<isl::aff> &affineIndices);

[[nodiscard]] isl::aff
CalculateRowMajorAffineAddress(size_t startingAddress, size_t elementSize,
                               const std::vector<size_t> &arrayDims,
                               const std::vector<isl::aff> &affineIndices);

[[nodiscard]] bool
AreIntervalsDisjoint(const std::pair<size_t, size_t> &interval1,
                     const std::pair<size_t, size_t> &interval2);

[[nodiscard]] bool IsInInterval(size_t val,
                                const std::pair<size_t, size_t> &interval);

[[nodiscard]] std::pair<size_t, size_t>
GetLeftHalfInterval(const std::pair<size_t, size_t> &interval);

[[nodiscard]] std::pair<size_t, size_t>
GetRightHalfInterval(const std::pair<size_t, size_t> &interval);

[[nodiscard]] bool IsPowerOfTwo(size_t n);

void HashCombine(size_t hash, std::size_t &seed);

[[nodiscard]] size_t AlignAddressToCacheLine(size_t address);

[[nodiscard]] size_t ComputeMemoryBlock(size_t address);

[[nodiscard]] long ComputeMemoryBlockDelta(long addressDelta);

[[nodiscard]] size_t ComputeNumberOfCacheLines(size_t cacheSize,
                                               size_t lineSize);

[[nodiscard]] size_t ComputeNumberOfCacheSets(size_t nCacheLines,
                                              size_t associativity);

[[nodiscard]] size_t ComputeNumberOfCacheSets(size_t cacheSize, size_t lineSize,
                                              size_t associativity);

[[nodiscard]] bool DoesFileExist(const std::string &fileName);

}; // namespace Util
