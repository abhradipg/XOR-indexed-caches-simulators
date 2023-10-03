#include "ConditionCompliesWarpingChecker.hpp"

#include <utility>

ConditionCompliesWarpingChecker::ConditionCompliesWarpingChecker(
    std::set<size_t> conditionIteratorIds, const isl::set &conditionDomain,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap),
      conditionIteratorIds(std::move(conditionIteratorIds)),
      conditionDomain(conditionDomain),
      conditionString(IslUtil::GetConditionString(this->conditionDomain)),
      conditionQueryBaseString(this->ConstructConditionQueryBaseString()) {}

size_t ConditionCompliesWarpingChecker::ComputeIterationsConditionComplies(
    const SnapshotMatch &sm) const {
  const auto itId = sm.GetIteratorId();
  const auto &itState = this->iteratorStateMap.GetIteratorState(itId);
  const auto &itUniqueName = itState->GetIteratorUniqueName();
  const auto &queryStr = this->ConstructConditionCompliesQueryStr(sm);
  const auto &querySet = isl::basic_set(this->conditionDomain.ctx(), queryStr);
  assert(!querySet.is_null());

  if (querySet.is_empty())
    return itState->GetNumberOfRemainingIterations();

  auto itPos = IslUtil::GetBasicSetDimNamePos(querySet, itUniqueName);
  const auto nearestFailingItVal =
      itState->IsIncreasing()
          ? IslUtil::GetBasicSetDimMinValue(querySet, itPos)
          : IslUtil::GetBasicSetDimMaxValue(querySet, itPos);
  return itState->GetNumberOfIterationsUntilIteratorValue(nearestFailingItVal);
}

std::string ConditionCompliesWarpingChecker::ConstructConditionCompliesQueryStr(
    const SnapshotMatch &sm) const {
  // TODO: refactor... use parametrized query instead of constructing a str..
  assert(sm.GetIteratorId() == this->iteratorStateMap.GetChangingIteratorId());
  assert(this->conditionIteratorIds.count(sm.GetIteratorId()) == 1);
  const auto nMatch = sm.GetNIterationsBetween();
  const auto oldItVal = sm.GetOlderItVal();
  const auto newItVal = sm.GetNewerItVal();
  const auto isIncreasing = oldItVal < newItVal;
  auto queryStr = this->conditionQueryBaseString;

  const auto &changingItId = this->iteratorStateMap.GetChangingIteratorId();
  const auto &changingItUniqueName =
      this->iteratorStateMap.GetChangingIteratorUniqueName();

  for (const auto itId : this->conditionIteratorIds) {
    auto valStr =
        std::to_string(this->iteratorStateMap.GetIteratorCurrentValue(itId));
    queryStr.append(" and ").append(
        this->iteratorStateMap.GetIteratorUniqueName(itId));
    if (itId == changingItId)
      queryStr.append(isIncreasing ? " > " : " < ");
    else
      queryStr.append(" = ");
    queryStr.append(valStr);
  }

  constexpr auto format = "(%d + ((%s - %d) mod %d))";
  const auto &expr = StrUtil::Format(
      format, oldItVal, changingItUniqueName.c_str(), newItVal, nMatch);

  const auto &replacedCondStr = IslUtil::ReplaceIteratorWithExpressionString(
      changingItUniqueName, expr, this->conditionString);
  const auto &iffCond = IslUtil::ConstructIffConditionString(
      this->conditionString, replacedCondStr);
  const auto &negIffCond = IslUtil::NegateConditionString(iffCond);

  queryStr.append(" and ").append(negIffCond).append("}");
  return queryStr;
}

std::string
ConditionCompliesWarpingChecker::ConstructConditionQueryBaseString() const {
  auto domStr = IslUtil::ToString(this->conditionDomain);
  auto closingBracePos = domStr.find('}');
  assert(closingBracePos != std::string::npos);
  return domStr.substr(0, closingBracePos);
}
