#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <set>

#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/AccessSimulationNode.hpp"
#include "Simulation/NodeId.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Simulation/SimulationNode.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Util/StrUtil.hpp"
#include "Warping/ConditionCompliesWarpingChecker.hpp"

class ConditionalConflictManager {
public:
  ConditionalConflictManager(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos,
      const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool HasDependingConditions() const;

  [[nodiscard]] size_t ComputeIterationsToWarp(const SnapshotMatch &sm) const;

private:
  const IteratorStateMap &iteratorStateMap;

  std::vector<ConditionCompliesWarpingChecker> checkers;

  void InitCheckers(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                        &simulationAccessInfos);
};
