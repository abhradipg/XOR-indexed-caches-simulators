#include "ArrayInfoExtractor.hpp"

ArrayInfoExtractor::ArrayInfoExtractor(pet_scop *scop) {
  this->InitArrayDims(scop);
  this->InitArrayInfos(scop);
}

const std::unordered_map<std::string, ArrayInfo> &
ArrayInfoExtractor::GetArrayInfos() const {
  return this->arrayInfos;
}

void ArrayInfoExtractor::InitArrayDims(pet_scop *scop) {
  this->InitArrayDimsViaPetArrayExtents(scop);
  this->InitUnboundedArrayDimsViaAccessDomains(scop);
}

void ArrayInfoExtractor::InitArrayInfos(pet_scop *scop) {
  size_t startingAddress = 0;
  for (int i = 0; i < scop->n_array; i++) {
    const auto array = scop->arrays[i];
    const auto extent = isl::manage_copy(array->extent);
    if (IslUtil::IsArrayExtent(extent)) {
      const auto &name = IslUtil::GetSetTupleName(extent);
      assert(this->arrayInfos.count(name) == 0);
      const auto dimInfoIt = this->arrayDimInfos.find(name);
      assert(dimInfoIt != this->arrayDimInfos.end());
      assert(array->element_size > 0);
      const auto &ai = ArrayInfo(array->element_size, startingAddress,
                                 dimInfoIt->second.GetDims());
      this->arrayInfos.emplace(name, ai);
      startingAddress += ai.GetSize();
      startingAddress = Util::AlignAddressToCacheLine(startingAddress);
      assert(startingAddress % GlobalVars::lineSize == 0);
    }
  }
}

void ArrayInfoExtractor::InitArrayDimsViaPetArrayExtents(pet_scop *scop) {
  for (int i = 0; i < scop->n_array; i++) {
    const auto array = scop->arrays[i];
    const auto extent = isl::manage_copy(array->extent);
    if (IslUtil::IsArrayExtent(extent)) {
      const auto &name = IslUtil::GetSetTupleName(extent);
      assert(this->arrayDimInfos.count(name) == 0);
      this->arrayDimInfos.emplace(name, IslUtil::ConvertSetToBasicSet(extent));
    }
  }
}

void ArrayInfoExtractor::InitUnboundedArrayDimsViaAccessDomains(
    pet_scop *scop) {
  const auto processTaggedAccess = [&](const isl::map &taggedAccess) {
    const auto &accessDom = IslUtil::ConvertSetToBasicSet(taggedAccess.range());
    const auto nDims = IslUtil::GetBasicSetDimension(accessDom);
    const auto isArrayAccess = nDims > 0;
    if (isArrayAccess) {
      const auto &arrayName = IslUtil::GetSetTupleName(accessDom);
      for (size_t i = 0; i < nDims; i++) {
        const auto dimInfoIt = this->arrayDimInfos.find(arrayName);
        assert(dimInfoIt != this->arrayDimInfos.end());
        const auto indexMax = IslUtil::GetBasicSetDimMaxValue(accessDom, i);
        assert(indexMax >= 0);
        const size_t dimMax = indexMax + 1;
        dimInfoIt->second.UpdateDimIfGreater(i, dimMax);
      }
    }
  };

  PetUtil::GetTaggedMayReadsAndWrites(scop).foreach_map(processTaggedAccess);
}

ArrayInfoExtractor::ArrayDimInfo::ArrayDimInfo(
    const isl::basic_set &arrayExtent) {
  assert(IslUtil::IsArrayExtent(arrayExtent));
  const auto nDims = IslUtil::GetBasicSetDimension(arrayExtent);
  assert(nDims > 0);
  this->dims.resize(nDims, 0);
  for (size_t i = 0; i < nDims; i++) {
    if (IslUtil::IsBasicSetDimMaxValBounded(arrayExtent, i)) {
      const auto indexMax = IslUtil::GetBasicSetDimMaxValue(arrayExtent, i);
      assert(indexMax >= 0);
      this->dims[i] = (size_t)indexMax + 1;
    }
  }
}

const std::vector<size_t> &ArrayInfoExtractor::ArrayDimInfo::GetDims() const {
  return this->dims;
}

void ArrayInfoExtractor::ArrayDimInfo::UpdateDimIfGreater(size_t dimPos,
                                                          size_t dim) {
  if (dim > this->dims[dimPos])
    this->dims[dimPos] = dim;
}
