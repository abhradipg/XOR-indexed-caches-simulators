#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Warping/CoefficientConflict.hpp"

class CoefficientConflictManager {
public:
  CoefficientConflictManager(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos,
      const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool HasCoefficientConflicts() const;

  [[nodiscard]] size_t
  ComputeIterationsToWarp(const SnapshotMatch &snapshotMatch) const;

private:
  const IteratorStateMap &iteratorStateMap;

  std::vector<CoefficientConflict> conflicts;

  void InitConflicts(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                         &simulationAccessInfos);

  [[nodiscard]] std::vector<std::shared_ptr<SimulationAccessInfo>>
  FilterDuplicateAccesses(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos) const;

  [[nodiscard]] std::unordered_map<
      long, std::vector<std::shared_ptr<SimulationAccessInfo>>>
  ExtractSimulationAccessInfosByCoefficient(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos) const;

  void AddConflictsForDiffCoefficientAccesses(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos1,
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos2);
};
