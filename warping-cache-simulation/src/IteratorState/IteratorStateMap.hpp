#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "IteratorState.hpp"

class IteratorState;

class IteratorStateMap {
public:
  [[nodiscard]] size_t GetChangingIteratorId() const;

  [[nodiscard]] size_t GetIteratorId(const std::string &itUniqueName) const;

  [[nodiscard]] long GetChangingIteratorCurrentValue() const;

  [[nodiscard]] long GetChangingIteratorStride() const;

  [[nodiscard]] const std::shared_ptr<IteratorState> &
  GetChangingIteratorState() const;

  [[nodiscard]] bool IsChangingIteratorIncreasing() const;

  [[nodiscard]] bool ContainsIterator(size_t itId) const;

  [[nodiscard]] bool IsOuterIteratorOfChangingIterator(size_t itId) const;

  [[nodiscard]] long GetIteratorCurrentValue(size_t itId) const;

  [[nodiscard]] const std::string &GetChangingIteratorName() const;

  [[nodiscard]] const std::string &GetChangingIteratorUniqueName() const;

  [[nodiscard]] const std::shared_ptr<IteratorState> &
  GetIteratorState(size_t itId) const;

  [[nodiscard]] const std::unordered_map<size_t, std::shared_ptr<IteratorState>>
      &GetIteratorStates() const;

  [[nodiscard]] const std::unordered_map<size_t, const std::shared_ptr<long>> &
  GetCurrentIteratorValuePointers() const;

  [[nodiscard]] const std::unordered_map<std::string, std::string> &
  GetIteratorNamesToIteratorUniqueNames() const;

  [[nodiscard]] const std::unordered_map<std::string, size_t> &
  GetIteratorUniqueNamesToIteratorIds() const;

  [[nodiscard]] const std::unordered_map<size_t, std::string> &
  GetIteratorIdsToIteratorUniqueNames() const;

  [[nodiscard]] const std::string &GetIteratorUniqueName(size_t itId) const;

  [[nodiscard]] const std::string &GetIteratorName(size_t itId) const;

  void
  AddNewChangingIterator(const std::shared_ptr<IteratorState> &iteratorState);

  void MarkAsWarpingInapplicable(size_t itId);

private:
  std::unordered_map<size_t, std::shared_ptr<IteratorState>> iteratorStates;

  std::unordered_map<size_t, std::string> iteratorIdsToIteratorUniqueNames;

  std::unordered_map<size_t, const std::shared_ptr<long>>
      currentIteratorValuePointers;

  std::unordered_map<std::string, std::string>
      iteratorNamesToIteratorUniqueNames;

  std::unordered_map<std::string, size_t> iteratorUniqueNamesToIteratorIds;

  size_t changingIteratorId;
};
