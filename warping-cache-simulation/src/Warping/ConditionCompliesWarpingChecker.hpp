#pragma once

#include <isl/cpp.h>

#include "IteratorState/IteratorStateMap.hpp"
#include "Snapshot/SnapshotMatch.hpp"

class ConditionCompliesWarpingChecker {
public:
  ConditionCompliesWarpingChecker(std::set<size_t> conditionIteratorIds,
                                  const isl::set &conditionDomain,
                                  const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] size_t
  ComputeIterationsConditionComplies(const SnapshotMatch &sm) const;

private:
  const IteratorStateMap &iteratorStateMap;
  const std::set<size_t> conditionIteratorIds;
  const isl::set conditionDomain;
  const std::string conditionString;
  const std::string conditionQueryBaseString;

  [[nodiscard]] std::string ConstructConditionQueryBaseString() const;

  [[nodiscard]] std::string
  ConstructConditionCompliesQueryStr(const SnapshotMatch &sm) const;
};
