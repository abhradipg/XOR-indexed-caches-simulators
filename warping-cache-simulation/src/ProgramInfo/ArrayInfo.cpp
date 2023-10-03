#include "ArrayInfo.hpp"

ArrayInfo::ArrayInfo(size_t elementSize, size_t startingAddress,
                     std::vector<size_t> dims)
    : elementSize(elementSize), startingAddress(startingAddress),
      dims(std::move(dims)), size(CalculateArraySize()) {}

size_t ArrayInfo::GetElementSize() const { return this->elementSize; }

size_t ArrayInfo::GetStartingAddress() const { return this->startingAddress; }

size_t ArrayInfo::GetSize() const { return this->size; }

const std::vector<size_t> &ArrayInfo::GetDims() const { return this->dims; }

isl::aff
ArrayInfo::CreateAffAddress(const std::vector<isl::aff> &affineIndices) const {
  return Util::CalculateAffineAddress(this->startingAddress, this->elementSize,
                                      this->dims, affineIndices);
}

bool ArrayInfo::IsDisjoint(const ArrayInfo &other) const {
  size_t start1 = this->startingAddress, start2 = other.startingAddress;
  size_t end1 = start1 + this->size, end2 = start2 + other.size;
  return Util::AreIntervalsDisjoint({start1, end1}, {start2, end2});
}

size_t ArrayInfo::CalculateArraySize() const {
  size_t res = 1;
  for (auto dim : this->dims)
    res *= dim;
  res *= this->elementSize;
  return res;
}
