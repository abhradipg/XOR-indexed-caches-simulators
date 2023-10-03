#include "CoefficientConflict.hpp"

CoefficientConflict::CoefficientConflict(
    const SimulationAccessInfo &sai1, const SimulationAccessInfo &sai2,
    const IteratorStateMap &iteratorStateMap)
    : context(sai1.GetAccessDomain().ctx()),
      iteratorStateMap(iteratorStateMap) {
  assert(!sai1.IsAccessedArrayMemoryBlockIntervalDisjoint(sai2));
  this->InitConflictSetStringBasis(sai1, sai2);
  this->InitConflictSetBasis();
}

bool CoefficientConflict::IsEmpty() const {
  return this->conflictSetBasis.is_empty();
}

size_t CoefficientConflict::ComputeIterationsUntilNextConflict(
    const SnapshotMatch &snapshotMatch) const {
  assert(!this->IsEmpty());
  const auto &querySetStr = this->ConstructNextConflictQueryStr(snapshotMatch);
  const auto &querySet = isl::basic_set(this->context, querySetStr);
  assert(!querySet.is_null());

  const auto itId = snapshotMatch.GetIteratorId();
  const auto &itState = this->iteratorStateMap.GetIteratorState(itId);
  assert(itState->IsIncreasing() == snapshotMatch.IsItIncreasing());

  if (querySet.is_empty())
    return itState->GetNumberOfRemainingIterations();

  const auto conflictItVal = snapshotMatch.IsItIncreasing()
                                 ? IslUtil::GetBasicSetDimMinValue(querySet, 0)
                                 : IslUtil::GetBasicSetDimMaxValue(querySet, 0);

  if (conflictItVal <= itState->GetCurrentValue())
    return 0;

  return itState->GetNumberOfIterationsUntilIteratorValue(conflictItVal);
}

void CoefficientConflict::InitConflictSetStringBasis(
    const SimulationAccessInfo &sai1, const SimulationAccessInfo &sai2) {
  this->conflictSetStringBasis = IslUtil::CreateConflictSetStringBasis(
      sai1.GetAffineAddress(), sai2.GetAffineAddress(), sai1.GetAccessDomain(),
      sai2.GetAccessDomain(), APPEND1, APPEND2);

  const auto &changingItUniqueName =
      this->iteratorStateMap.GetChangingIteratorUniqueName();
  const std::string &op =
      this->iteratorStateMap.IsChangingIteratorIncreasing() ? " >= " : " <= ";

  const auto &cond1 = " and x " + op + changingItUniqueName + APPEND1;
  const auto &cond2 = " and x " + op + changingItUniqueName + APPEND2;
  this->conflictSetStringBasis.append(cond1);
  this->conflictSetStringBasis.append(cond2);
}

void CoefficientConflict::InitConflictSetBasis() {
  this->conflictSetBasis =
      isl::basic_set(this->context, this->conflictSetStringBasis + "}");
}

std::string CoefficientConflict::ConstructNextConflictQueryStr(
    const SnapshotMatch &snapshotMatch) const {
  // TODO: refactor...
  assert(snapshotMatch.GetIteratorId() ==
         this->iteratorStateMap.GetChangingIteratorId());
  const auto changingItId = this->iteratorStateMap.GetChangingIteratorId();
  const auto &changingItName = this->iteratorStateMap.GetChangingIteratorName();
  const auto &changingItUniqueName =
      this->iteratorStateMap.GetChangingIteratorUniqueName();
  const auto &startStr = (snapshotMatch.IsItIncreasing() ? " >= " : " <= ") +
                         std::to_string(snapshotMatch.GetOlderItVal());

  auto changingItCond1 = changingItUniqueName + APPEND1 + startStr;
  auto changingItCond2 = changingItUniqueName + APPEND2 + startStr;

  std::string querySetStr = this->conflictSetStringBasis;
  querySetStr.append(" and ").append(changingItCond1);
  querySetStr.append(" and ").append(changingItCond2);

  for (auto &el : this->iteratorStateMap.GetIteratorStates()) {
    const auto itId = el.first;
    const auto &itState = el.second;
    const auto &itName = itState->GetIteratorName();
    if (itId == changingItId)
      continue;
    // add only parent (outer loop) iterator values
    if (StrUtil::IsOuterIteratorName(itName, changingItName))
      continue;

    const auto &itUniqueName = itState->GetIteratorUniqueName();
    const auto itVal = itState->GetCurrentValue();
    assert(el.second != nullptr);
    querySetStr.append(" and ")
        .append(itUniqueName)
        .append(APPEND1)
        .append(" = ")
        .append(std::to_string(itVal));
    querySetStr.append(" and ")
        .append(itUniqueName)
        .append(APPEND2)
        .append(" = ")
        .append(std::to_string(itVal));
  }
  querySetStr.append(" }");
  return querySetStr;
}
