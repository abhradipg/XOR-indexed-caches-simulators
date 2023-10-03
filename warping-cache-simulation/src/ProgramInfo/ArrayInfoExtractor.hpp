#pragma once

#include <algorithm>
#include <isl/cpp.h>
#include <pet.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "ProgramInfo/ArrayInfo.hpp"
#include "Util/PetUtil.hpp"

class ArrayInfoExtractor {
public:
  explicit ArrayInfoExtractor(pet_scop *scop);

  [[nodiscard]] const std::unordered_map<std::string, ArrayInfo> &
  GetArrayInfos() const;

private:
  struct ArrayDimInfo {
  public:
    explicit ArrayDimInfo(const isl::basic_set &arrayExtent);

    [[nodiscard]] const std::vector<size_t> &GetDims() const;

    void UpdateDimIfGreater(size_t dimPos, size_t dim);

  private:
    std::vector<size_t> dims;
  };

  std::unordered_map<std::string, ArrayDimInfo> arrayDimInfos;
  std::unordered_map<std::string, ArrayInfo> arrayInfos;

  void InitArrayDims(pet_scop *scop);

  void InitArrayInfos(pet_scop *scop);

  void InitArrayDimsViaPetArrayExtents(pet_scop *scop);

  void InitUnboundedArrayDimsViaAccessDomains(pet_scop *scop);
};
