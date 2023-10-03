#include "IteratorBound.hpp"

IteratorBound::IteratorBound(const isl::set &set,
                             const std::string &itUniqueName, ItIdMap itIdMap)
    : IteratorBound(IteratorBoundExtractor(set, itUniqueName, itIdMap)) {}

IteratorBound::IteratorBound(const IteratorBoundExtractor &ibe)
    : lowerBound(ibe.GetLowerBound()), upperBound(ibe.GetUpperBound()),
      isIncreasing(ibe.IsIncreasing()) {}

bool IteratorBound::HasConstantNumberOfIterations() const {
  return this->lowerBound->IsConstant() && this->upperBound->IsConstant();
}

std::set<size_t> IteratorBound::GetAffectingItIds() const {
  auto ids = this->lowerBound->GetAffectingVarIds();
  ids.merge(this->upperBound->GetAffectingVarIds());
  return ids;
}

std::pair<long, long>
IteratorBound::GetFirstAndLastValue(ItValMap itValMap) const {
  const auto &minAndMax = this->GetMinAndMaxValue(itValMap);
  return this->isIncreasing ? std::make_pair(minAndMax.first, minAndMax.second)
                            : std::make_pair(minAndMax.second, minAndMax.first);
}

std::pair<long, long>
IteratorBound::GetMinAndMaxValue(ItValMap itValMap) const {
  return {this->lowerBound->GetCurrentValue(itValMap),
          this->upperBound->GetCurrentValue(itValMap)};
}
