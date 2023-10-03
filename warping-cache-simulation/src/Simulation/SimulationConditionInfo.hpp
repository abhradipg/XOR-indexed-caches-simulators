#pragma once

#include <isl/cpp.h>
#include <set>
#include <string>
#include <vector>

#include "IteratorState/IteratorConditionInfo.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/ConditionHoldsChecker.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Util/IslUtil.hpp"
#include "Util/StrUtil.hpp"

class SimulationConditionInfo {
public:
  SimulationConditionInfo(const isl::set &domain, const isl::set &parentDomain,
                          const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool IsConditional() const;

  [[nodiscard]] bool DoesConditionHold() const;

  [[nodiscard]] bool DoesConditionDependOnIterator(size_t itId) const;

  [[nodiscard]] const isl::set &GetConditionDomain() const;

  [[nodiscard]] const std::set<size_t> &GetConditionIteratorIds() const;

  [[nodiscard]] const IteratorStateMap &GetIteratorStateMap() const;

private:
  const IteratorStateMap &iteratorStateMap;
  const isl::set domain;
  bool isConditional = false;
  std::set<size_t> conditionIteratorIds;
  std::optional<ConditionHoldsChecker> conditionHoldsChecker;

  void InitForOutOfLoopAccess();
  void InitForInLoopAccess(const isl::set &parentDomain);
  void InitConditionalFields();
  void InitConditionIteratorIds();
  void InitConditionHoldsChecker();
};
