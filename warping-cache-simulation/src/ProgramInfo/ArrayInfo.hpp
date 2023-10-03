#pragma once

#include <isl/cpp.h>
#include <vector>

#include "Util/IslUtil.hpp"

class ArrayInfo {
public:
  ArrayInfo(size_t elementSize, size_t startingAddress,
            std::vector<size_t> dims);

  [[nodiscard]] size_t GetElementSize() const;

  [[nodiscard]] size_t GetStartingAddress() const;

  [[nodiscard]] size_t GetSize() const;

  [[nodiscard]] const std::vector<size_t> &GetDims() const;

  [[nodiscard]] isl::aff
  CreateAffAddress(const std::vector<isl::aff> &affineIndices) const;

  [[nodiscard]] bool IsDisjoint(const ArrayInfo &other) const;

private:
  size_t elementSize;
  size_t startingAddress;
  std::vector<size_t> dims;
  size_t size;

  [[nodiscard]] size_t CalculateArraySize() const;
};
