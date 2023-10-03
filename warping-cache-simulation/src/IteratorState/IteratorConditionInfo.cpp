#include "IteratorConditionInfo.hpp"

IteratorConditionInfo::IteratorConditionInfo(
    size_t itId, const isl::basic_set &conditionDomain,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap),
      iteratorState(*iteratorStateMap.GetIteratorState(itId)),
      constLowerBound(IteratorConditionInfo::ExtractConstLowerBound(
          conditionDomain, this->iteratorState.GetIteratorUniqueName())),
      constUpperBound(IteratorConditionInfo::ExtractConstUpperBound(
          conditionDomain, this->iteratorState.GetIteratorUniqueName())),
      affineLowerBounds(IteratorConditionInfo::ExtractAffineLowerBounds(
          conditionDomain, this->iteratorState.GetIteratorUniqueName())),
      affineUpperBounds(IteratorConditionInfo::ExtractAffineUpperBounds(
          conditionDomain, this->iteratorState.GetIteratorUniqueName())) {
  assert(this->constLowerBound || this->constUpperBound ||
         !this->affineLowerBounds.empty() || !this->affineUpperBounds.empty());
}

bool IteratorConditionInfo::DoesConditionCurrentlyFail() const {
  return this->DoesFailConstLowerBound() || this->DoesFailConstUpperBound() ||
         this->DoesFailAnyAffLowerBound() || this->DoesFailAnyAffUpperBound();
}

bool IteratorConditionInfo::DoesFailConstLowerBound() const {
  const auto itVal = this->iteratorState.GetCurrentValue();
  return this->constLowerBound && itVal < *this->constLowerBound;
}

bool IteratorConditionInfo::DoesFailConstUpperBound() const {
  const auto itVal = this->iteratorState.GetCurrentValue();
  return this->constUpperBound && itVal > *this->constUpperBound;
}

bool IteratorConditionInfo::DoesFailAnyAffLowerBound() const {
  const auto itVal = this->iteratorState.GetCurrentValue();
  const auto &itValMap =
      this->iteratorStateMap.GetCurrentIteratorValuePointers();
  return std::any_of(this->affineLowerBounds.begin(),
                     this->affineLowerBounds.end(),
                     [&](const auto &af) { return itVal < af.Eval(itValMap); });
}

bool IteratorConditionInfo::DoesFailAnyAffUpperBound() const {
  const auto itVal = this->iteratorState.GetCurrentValue();
  const auto &itValMap =
      this->iteratorStateMap.GetCurrentIteratorValuePointers();
  return std::any_of(this->affineUpperBounds.begin(),
                     this->affineUpperBounds.end(),
                     [&](const auto &af) { return itVal > af.Eval(itValMap); });
}

std::optional<long> IteratorConditionInfo::ExtractConstLowerBound(
    const isl::basic_set &conditionDomain, const std::string &dimName) {
  const auto &bounds =
      IslUtil::ExtractConstantLowerBounds(conditionDomain, dimName);
  const auto it = std::min_element(bounds.begin(), bounds.end());
  if (it == bounds.end())
    return std::nullopt;
  return std::make_optional(*it);
}

std::optional<long> IteratorConditionInfo::ExtractConstUpperBound(
    const isl::basic_set &conditionDomain, const std::string &dimName) {
  const auto &bounds =
      IslUtil::ExtractConstantUpperBounds(conditionDomain, dimName);
  const auto it = std::max_element(bounds.begin(), bounds.end());
  if (it == bounds.end())
    return std::nullopt;
  return std::make_optional(*it);
}

std::vector<AffineFunction> IteratorConditionInfo::ExtractAffineLowerBounds(
    const isl::basic_set &conditionDomain, const std::string &dimName) const {
  std::vector<AffineFunction> res;
  for (const auto &aff :
       IslUtil::ExtractAffLowerBounds(conditionDomain, dimName))
    res.emplace_back(
        aff, this->iteratorStateMap.GetIteratorUniqueNamesToIteratorIds());
  return res;
}

std::vector<AffineFunction> IteratorConditionInfo::ExtractAffineUpperBounds(
    const isl::basic_set &conditionDomain, const std::string &dimName) const {
  std::vector<AffineFunction> res;
  for (const auto &aff :
       IslUtil::ExtractAffUpperBounds(conditionDomain, dimName))
    res.emplace_back(
        aff, this->iteratorStateMap.GetIteratorUniqueNamesToIteratorIds());
  return res;
}
