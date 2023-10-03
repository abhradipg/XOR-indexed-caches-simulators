#include "IteratorStateMap.hpp"

size_t IteratorStateMap::GetChangingIteratorId() const {
  return this->changingIteratorId;
}

size_t IteratorStateMap::GetIteratorId(const std::string &itUniqueName) const {
  const auto it = this->iteratorUniqueNamesToIteratorIds.find(itUniqueName);
  assert(it != this->iteratorUniqueNamesToIteratorIds.end());
  return it->second;
}

long IteratorStateMap::GetChangingIteratorCurrentValue() const {
  return this->GetChangingIteratorState()->GetCurrentValue();
}

long IteratorStateMap::GetChangingIteratorStride() const {
  return this->GetChangingIteratorState()->GetStride();
}

const std::shared_ptr<IteratorState> &
IteratorStateMap::GetChangingIteratorState() const {
  return this->GetIteratorState(this->GetChangingIteratorId());
}

bool IteratorStateMap::IsChangingIteratorIncreasing() const {
  return this->GetChangingIteratorState()->IsIncreasing();
}

bool IteratorStateMap::ContainsIterator(size_t itId) const {
  return this->iteratorStates.count(itId) == 1;
}

bool IteratorStateMap::IsOuterIteratorOfChangingIterator(size_t itId) const {
  return itId != this->changingIteratorId && this->ContainsIterator(itId);
}

long IteratorStateMap::GetIteratorCurrentValue(size_t itId) const {
  return this->GetIteratorState(itId)->GetCurrentValue();
}

const std::string &IteratorStateMap::GetChangingIteratorName() const {
  return this->GetChangingIteratorState()->GetIteratorName();
}

const std::string &IteratorStateMap::GetChangingIteratorUniqueName() const {
  return this->GetChangingIteratorState()->GetIteratorUniqueName();
}

const std::shared_ptr<IteratorState> &
IteratorStateMap::GetIteratorState(size_t itId) const {
  assert(this->ContainsIterator(itId));
  return this->iteratorStates.at(itId);
}

const std::unordered_map<size_t, std::shared_ptr<IteratorState>> &
IteratorStateMap::GetIteratorStates() const {
  return this->iteratorStates;
}

const std::unordered_map<size_t, const std::shared_ptr<long>> &
IteratorStateMap::GetCurrentIteratorValuePointers() const {
  return this->currentIteratorValuePointers;
}

const std::unordered_map<std::string, std::string> &
IteratorStateMap::GetIteratorNamesToIteratorUniqueNames() const {
  return this->iteratorNamesToIteratorUniqueNames;
}

const std::unordered_map<std::string, size_t> &
IteratorStateMap::GetIteratorUniqueNamesToIteratorIds() const {
  return this->iteratorUniqueNamesToIteratorIds;
}

const std::unordered_map<size_t, std::string> &
IteratorStateMap::GetIteratorIdsToIteratorUniqueNames() const {
  return this->iteratorIdsToIteratorUniqueNames;
}

const std::string &IteratorStateMap::GetIteratorUniqueName(size_t itId) const {
  const auto it = this->iteratorIdsToIteratorUniqueNames.find(itId);
  assert(it != this->iteratorIdsToIteratorUniqueNames.end());
  assert(!it->second.empty());
  return it->second;
}

const std::string &IteratorStateMap::GetIteratorName(size_t itId) const {
  const auto &itState = this->GetIteratorState(itId);
  assert(itState != nullptr);
  return itState->GetIteratorName();
}

void IteratorStateMap::AddNewChangingIterator(
    const std::shared_ptr<IteratorState> &iteratorState) {
  const auto itId = iteratorState->GetIteratorId();
  const auto &itName = iteratorState->GetIteratorName();
  const auto &itUniqueName = iteratorState->GetIteratorUniqueName();
  assert(!this->ContainsIterator(itId));
  assert(this->iteratorNamesToIteratorUniqueNames.count(itName) == 0);
  this->changingIteratorId = itId;
  this->iteratorStates.emplace(itId, iteratorState);
  this->currentIteratorValuePointers.emplace(
      itId, iteratorState->GetCurrentValuePointer());
  this->iteratorNamesToIteratorUniqueNames.emplace(itName, itUniqueName);
  this->iteratorUniqueNamesToIteratorIds.emplace(itUniqueName, itId);
  this->iteratorIdsToIteratorUniqueNames.emplace(itId, itUniqueName);
}

void IteratorStateMap::MarkAsWarpingInapplicable(size_t itId) {
  this->GetIteratorState(itId)->MarkAsWarpingInapplicable();
}
