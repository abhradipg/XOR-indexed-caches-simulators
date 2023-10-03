#include "Util.hpp"

isl::aff
Util::CalculateAffineAddress(size_t startingAddress, size_t elementSize,
                             const std::vector<size_t> &arrayDims,
                             const std::vector<isl::aff> &affineIndices) {
  return Util::CalculateRowMajorAffineAddress(startingAddress, elementSize,
                                              arrayDims, affineIndices);
}

isl::aff Util::CalculateRowMajorAffineAddress(
    size_t startingAddress, size_t elementSize,
    const std::vector<size_t> &arrayDims,
    const std::vector<isl::aff> &affineIndices) {
  assert(affineIndices.size() == arrayDims.size());
  assert(!affineIndices.empty());
  isl::aff affineOffset;
  size_t nIndices = affineIndices.size();
  for (size_t i = 0; i < nIndices; i++) {
    isl::aff affineIndex = affineIndices[i];
    size_t coef = 1;
    for (size_t j = i + 1; j < nIndices; j++)
      coef *= arrayDims[j];
    affineOffset = affineOffset.is_null()
                       ? affineIndex.scale(coef)
                       : affineOffset.add(affineIndex.scale(coef));
  }
  affineOffset = affineOffset.scale(elementSize);
  return affineOffset.add_constant(startingAddress);
}

bool Util::AreIntervalsDisjoint(const std::pair<size_t, size_t> &interval1,
                                const std::pair<size_t, size_t> &interval2) {
  return interval1.first >= interval2.second ||
         interval2.first >= interval1.second;
}

bool Util::IsInInterval(size_t val, const std::pair<size_t, size_t> &interval) {
  return val >= interval.first && val <= interval.second;
}

std::pair<size_t, size_t>
Util::GetLeftHalfInterval(const std::pair<size_t, size_t> &interval) {
  assert(interval.second > interval.first);
  const auto len = interval.second - interval.first + 1;
  assert(len > 0 && len % 2 == 0);
  return {interval.first, interval.first + (len / 2) - 1};
}

std::pair<size_t, size_t>
Util::GetRightHalfInterval(const std::pair<size_t, size_t> &interval) {
  assert(interval.second > interval.first);
  const auto len = interval.second - interval.first + 1;
  assert(len > 0 && len % 2 == 0);
  return {interval.first + (len / 2), interval.second};
}

bool Util::IsPowerOfTwo(size_t n) { return n > 0 && (n & (n - 1)) == 0; }

void Util::HashCombine(size_t hash, std::size_t &seed) {
  seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

size_t Util::AlignAddressToCacheLine(size_t address) {
  if (address % GlobalVars::lineSize == 0)
    return address;
  const size_t currentLineIndex = address / GlobalVars::lineSize;
  return (currentLineIndex + 1) * GlobalVars::lineSize;
}

size_t Util::ComputeMemoryBlock(size_t address) {
  return (size_t)std::floor(address / GlobalVars::lineSize);
}

long Util::ComputeMemoryBlockDelta(long addressDelta) {
  const auto absMbDelta = Util::ComputeMemoryBlock(std::abs(addressDelta));
  const auto mbDelta = (long)absMbDelta * (addressDelta < 0 ? -1 : 1);
  return mbDelta;
}

size_t Util::ComputeNumberOfCacheLines(size_t cacheSize, size_t lineSize) {
  assert(cacheSize % lineSize == 0);
  return cacheSize / lineSize;
}

size_t Util::ComputeNumberOfCacheSets(size_t nCacheLines,
                                      size_t associativity) {
  assert(nCacheLines % associativity == 0);
  return nCacheLines / associativity;
}

size_t Util::ComputeNumberOfCacheSets(size_t cacheSize, size_t lineSize,
                                      size_t associativity) {
  return Util::ComputeNumberOfCacheSets(
      Util::ComputeNumberOfCacheLines(cacheSize, lineSize), associativity);
}

bool Util::DoesFileExist(const std::string &fileName) {
  std::ifstream test(fileName.c_str());
  return test.good();
}
