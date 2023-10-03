#include "FurthestWarpingPointManager.hpp"

FurthestWarpingPointManager::FurthestWarpingPointManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap),
      iteratorState(*iteratorStateMap.GetChangingIteratorState()),
      bijectionVerifier(simulationAccessInfos, iteratorStateMap),
      coefficientConflictManager(simulationAccessInfos, iteratorStateMap),
      conditionalConflictManager(simulationAccessInfos, iteratorStateMap) {}

size_t FurthestWarpingPointManager::ComputeIterationsToWarp(
    const SnapshotMatch &snapshotMatch) {
  const auto nMatch = snapshotMatch.GetNIterationsBetween();
  size_t nIterations = this->iteratorState.GetNumberOfRemainingIterations();

  if (this->ShouldCheckConditionalConflicts()) {
    const auto n = this->ComputeIterationsToWarpAccordingToConditionalConflicts(
        snapshotMatch);
    nIterations = std::min(nIterations, n);
    if (nIterations < nMatch)
      return 0;
  }

  if (this->ShouldCheckCoefficientConflicts()) {
    const auto n = this->ComputeIterationsToWarpAccordingToCoefficientConflicts(
        snapshotMatch);
    nIterations = std::min(nIterations, n);
    if (nIterations < nMatch)
      return 0;
  }

  if (this->ShouldCheckCacheBijection(snapshotMatch)) {
    if (!this->DoesCacheBijectionComplyWithAccessBijection(nIterations,
                                                           snapshotMatch))
      return 0;
  }

  return nIterations;
}

bool FurthestWarpingPointManager::ShouldCheckConditionalConflicts() const {
  return this->conditionalConflictManager.HasDependingConditions();
}

bool FurthestWarpingPointManager::ShouldCheckCoefficientConflicts() const {
  return this->coefficientConflictManager.HasCoefficientConflicts();
}

bool FurthestWarpingPointManager::ShouldCheckCacheBijection(
    const SnapshotMatch &snapshotMatch) const {
  return this->bijectionVerifier.ShouldCheckCacheBijection(snapshotMatch);
}

size_t FurthestWarpingPointManager::
    ComputeIterationsToWarpAccordingToConditionalConflicts(
        const SnapshotMatch &snapshotMatch) const {
  return this->conditionalConflictManager.ComputeIterationsToWarp(
      snapshotMatch);
}

size_t FurthestWarpingPointManager::
    ComputeIterationsToWarpAccordingToCoefficientConflicts(
        const SnapshotMatch &snapshotMatch) const {
  return this->coefficientConflictManager.ComputeIterationsToWarp(
      snapshotMatch);
}

bool FurthestWarpingPointManager::DoesCacheBijectionComplyWithAccessBijection(
    size_t iterationsToWarp, const SnapshotMatch &snapshotMatch) {
  const auto furthestItVal = this->iteratorState.GetCurrentValue() +
                             this->iteratorState.GetStride() * iterationsToWarp;
  return this->bijectionVerifier.DoesCacheBijectionComplyWithAccessBijection(
      furthestItVal, snapshotMatch);
}
