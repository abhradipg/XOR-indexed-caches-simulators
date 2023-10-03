#include "SimulationConditionInfo.hpp"

SimulationConditionInfo::SimulationConditionInfo(
    const isl::set &domain, const isl::set &parentDomain,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap), domain(domain) {
  const auto isForOutOfLoopAccess = parentDomain.is_empty();
  if (isForOutOfLoopAccess)
    this->InitForOutOfLoopAccess();
  else
    this->InitForInLoopAccess(parentDomain);
}

void SimulationConditionInfo::InitForOutOfLoopAccess() {
  this->isConditional = false;
}

void SimulationConditionInfo::InitForInLoopAccess(
    const isl::set &parentDomain) {
  assert(IslUtil::ConvertBasicSetTupleToStringVector(
             IslUtil::ConvertSetToBasicSet(this->domain)) ==
         IslUtil::ConvertBasicSetTupleToStringVector(
             IslUtil::ConvertSetToBasicSet(parentDomain)));
  this->isConditional = this->domain.is_strict_subset(parentDomain);

  if (this->isConditional)
    this->InitConditionalFields();
}

bool SimulationConditionInfo::IsConditional() const {
  return this->isConditional;
}

bool SimulationConditionInfo::DoesConditionHold() const {
  assert(this->isConditional && this->conditionHoldsChecker);
  return this->conditionHoldsChecker->DoesConditionHold();
}

bool SimulationConditionInfo::DoesConditionDependOnIterator(size_t itId) const {
  return this->conditionIteratorIds.count(itId) == 1;
}

const isl::set &SimulationConditionInfo::GetConditionDomain() const {
  return this->domain;
}

const std::set<size_t> &
SimulationConditionInfo::GetConditionIteratorIds() const {
  return this->conditionIteratorIds;
}

const IteratorStateMap &SimulationConditionInfo::GetIteratorStateMap() const {
  return this->iteratorStateMap;
}

void SimulationConditionInfo::InitConditionalFields() {
  this->InitConditionIteratorIds();
  this->InitConditionHoldsChecker();
}

void SimulationConditionInfo::InitConditionHoldsChecker() {
  assert(!this->conditionHoldsChecker);
  this->conditionHoldsChecker.emplace(ConditionHoldsChecker(
      this->conditionIteratorIds, this->domain, this->iteratorStateMap));
}

void SimulationConditionInfo::InitConditionIteratorIds() {
  const auto &domItUniqueNames =
      IslUtil::ConvertSetTupleToStringVector(this->domain);
  const auto &changingItName = this->iteratorStateMap.GetChangingIteratorName();
  for (const auto &itUniqueName : domItUniqueNames) {
    const auto itId = this->iteratorStateMap.GetIteratorId(itUniqueName);
    if (IslUtil::IsVarInConditionString(
            itUniqueName, IslUtil::GetConditionString(this->domain))) {
      /*
       * TODO:
       *  this if condition will always be true since the iterator
       *  is always in the domain, instead check if the iterator's domain is
       *  a strict subset
       */
      assert(this->conditionIteratorIds.count(itId) == 0);
      assert(!StrUtil::IsOuterIteratorName(
          this->iteratorStateMap.GetIteratorName(itId), changingItName));
      this->conditionIteratorIds.insert(itId);
    }
  }
}
