#include "GlobalIteratorState.hpp"

void GlobalIteratorState::Reserve(size_t nIterators) {
  this->iteratorUniqueNames.reserve(nIterators);
  this->iteratorValuePointers.reserve(nIterators);
}

const std::string &
GlobalIteratorState::GetIteratorUniqueName(size_t itId) const {
  assert(this->DoesContainIterator(itId) &&
         !this->iteratorUniqueNames[itId].empty());
  return this->iteratorUniqueNames[itId];
}

size_t
GlobalIteratorState::GetIteratorId(const std::string &itUniqueName) const {
  const auto it = this->iteratorIdsByIteratorUniqueNames.find(itUniqueName);
  assert(it != this->iteratorIdsByIteratorUniqueNames.end());
  return it->second;
}

long GlobalIteratorState::GetIteratorValue(size_t itId) const {
  assert(itId < this->iteratorValuePointers.size());
  assert(this->iteratorValuePointers[itId] != nullptr);
  return *this->iteratorValuePointers[itId];
}

const std::vector<std::string> &
GlobalIteratorState::GetIteratorUniqueNames() const {
  return this->iteratorUniqueNames;
}

const std::vector<std::shared_ptr<long>> &
GlobalIteratorState::GetIteratorValuePointers() const {
  return this->iteratorValuePointers;
}

void GlobalIteratorState::AddIterator(
    size_t itId, const std::string &itUniqueName,
    const std::shared_ptr<long> &itValPointer) {
  assert(itValPointer != nullptr);
  assert(itId == this->iteratorValuePointers.size());
  assert(this->iteratorValuePointers.size() ==
         this->iteratorUniqueNames.size());
  assert(this->iteratorIdsByIteratorUniqueNames.count(itUniqueName) == 0);
  this->iteratorValuePointers.push_back(itValPointer);
  this->iteratorUniqueNames.push_back(itUniqueName);
  this->iteratorIdsByIteratorUniqueNames.emplace(itUniqueName, itId);
}

bool GlobalIteratorState::DoesContainIterator(size_t itId) const {
  return itId < this->iteratorValuePointers.size();
}
