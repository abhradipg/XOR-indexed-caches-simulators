#pragma once

#include <memory>
#include <unordered_set>

#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Warping/BijectionVerifier.hpp"
#include "Warping/CoefficientConflictManager.hpp"
#include "Warping/ConditionalConflictManager.hpp"

class FurthestWarpingPointManager {
public:
  FurthestWarpingPointManager(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos,
      const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] size_t
  ComputeIterationsToWarp(const SnapshotMatch &snapshotMatch);

private:
  const IteratorStateMap &iteratorStateMap;
  const IteratorState &iteratorState;

  BijectionVerifier bijectionVerifier;
  CoefficientConflictManager coefficientConflictManager;
  ConditionalConflictManager conditionalConflictManager;

  [[nodiscard]] bool ShouldCheckConditionalConflicts() const;

  [[nodiscard]] bool ShouldCheckCoefficientConflicts() const;

  [[nodiscard]] bool
  ShouldCheckCacheBijection(const SnapshotMatch &snapshotMatch) const;

  [[nodiscard]] size_t ComputeIterationsToWarpAccordingToConditionalConflicts(
      const SnapshotMatch &snapshotMatch) const;

  [[nodiscard]] size_t ComputeIterationsToWarpAccordingToCoefficientConflicts(
      const SnapshotMatch &snapshotMatch) const;

  [[nodiscard]] bool DoesCacheBijectionComplyWithAccessBijection(
      size_t iterationsToWarp, const SnapshotMatch &snapshotMatch);
};
