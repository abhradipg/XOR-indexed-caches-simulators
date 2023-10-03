#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

class GlobalIteratorState {
public:
  void Reserve(size_t nIterators);

  [[nodiscard]] const std::string &GetIteratorUniqueName(size_t itId) const;

  [[nodiscard]] size_t GetIteratorId(const std::string &itUniqueName) const;

  [[nodiscard]] long GetIteratorValue(size_t itId) const;

  [[nodiscard]] const std::vector<std::string> &GetIteratorUniqueNames() const;

  [[nodiscard]] const std::vector<std::shared_ptr<long>> &
  GetIteratorValuePointers() const;

  void AddIterator(size_t itId, const std::string &itUniqueName,
                   const std::shared_ptr<long> &itValPointer);

private:
  std::vector<std::string> iteratorUniqueNames;

  std::unordered_map<std::string, size_t> iteratorIdsByIteratorUniqueNames;

  std::vector<std::shared_ptr<long>> iteratorValuePointers;

  [[nodiscard]] bool DoesContainIterator(size_t itId) const;
};
