#include "WarpingManager.hpp"

WarpingManager::WarpingManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap),
      iteratorState(iteratorStateMap.GetChangingIteratorState()),
      warpingPeriod(
          WarpingPeriodCalculator(simulationAccessInfos, *this->iteratorState)
              .GetWarpingPeriod()),
      snapshotManager(*this->iteratorState),
      furthestWarpingPointManager(simulationAccessInfos, iteratorStateMap) {
  assert(this->iteratorState->IsWarpingApplicable());
}

void WarpingManager::TryWarping(CacheState &cacheState,
                                SimulationResult &simulationResult) {
  if (!this->ShouldTryWarping(cacheState))
    return;

  this->OnInitialWarpTry(cacheState);

  const auto &sm = this->SaveSnapshotAndFindMatch(cacheState, simulationResult);
  if (sm == nullptr) {
    this->OnUnsuccessfulInitialWarp();
    return;
  }

  this->TryWarpingAccordingToSnapshotMatch(sm, cacheState, simulationResult);
}

bool WarpingManager::IsItTimeToWarpAccordingToPeriod() const {
  const auto nthIteration = this->iteratorState->GetNthIteration();
  return (nthIteration - 1) % this->warpingPeriod == 0;
}

void WarpingManager::ResetSnapshots() {
  this->snapshotManager.DeleteSnapshots();
}

std::shared_ptr<SnapshotMatch> WarpingManager::SaveSnapshotAndFindMatch(
    const CacheState &cacheState, const SimulationResult &simulationResult) {
  const auto &snapshot =
      this->snapshotManager.TakeSnapshot(simulationResult, cacheState);
  const auto &snapshotMatch = this->snapshotManager.FindMatch(snapshot);
  this->snapshotManager.AddSnapshot(snapshot);
  return snapshotMatch;
}

void WarpingManager::TryWarpingAccordingToSnapshotMatch(
    const std::shared_ptr<SnapshotMatch> &snapshotMatch, CacheState &cacheState,
    SimulationResult &simulationResult) {
  assert(snapshotMatch->GetIteratorId() ==
         this->iteratorStateMap.GetChangingIteratorId());

  const auto nWarp = this->TryInitialWarpingAndGetNWarp(
      *snapshotMatch, cacheState, simulationResult);

  if (nWarp == 0 || this->iteratorState->IsFinished()) {
    this->OnUnsuccessfulInitialWarp();
    return;
  }

  this->TryKeepWarping(nWarp, snapshotMatch, cacheState, simulationResult);
  assert(snapshotMatch->GetNewerItVal() ==
             this->iteratorState->GetCurrentValue() ||
         this->iteratorState->IsFinished());
}

size_t WarpingManager::TryInitialWarpingAndGetNWarp(
    const SnapshotMatch &snapshotMatch, CacheState &cacheState,
    SimulationResult &simulationResult) {
  const auto nWarp = this->ComputeNWarp(snapshotMatch);
  if (nWarp != 0)
    this->InitialWarp(this->CreateWarpInfo(nWarp, snapshotMatch), cacheState,
                      simulationResult);
  return nWarp;
}

size_t WarpingManager::TryWarpingAgainAndGetNWarp(
    const SnapshotMatch &lastSnapshotMatch, CacheState &cacheState,
    SimulationResult &simulationResult) {
  const auto nWarp = this->ComputeNWarp(lastSnapshotMatch);
  if (nWarp != 0)
    this->Warp(this->CreateWarpInfo(nWarp, lastSnapshotMatch), cacheState,
               simulationResult);
  return nWarp;
}

void WarpingManager::TryKeepWarping(
    size_t lastNWarp, const std::shared_ptr<SnapshotMatch> &lastSnapshotMatch,
    CacheState &cacheState, SimulationResult &simulationResult) {
  assert(lastNWarp > 0);
  const auto nMatch = lastSnapshotMatch->GetNIterationsBetween();
  while (lastNWarp > 0 &&
         nMatch <= this->iteratorState->GetNumberOfRemainingIterations()) {
    const auto itDiff = this->iteratorState->GetCurrentValue() -
                        lastSnapshotMatch->GetNewerItVal();
    assert(itDiff != 0);
    assert(itDiff == (long)lastNWarp * lastSnapshotMatch->GetItValDiff());
    lastSnapshotMatch->WarpBothSnapshots(lastNWarp);
    lastNWarp = this->TryWarpingAgainAndGetNWarp(*lastSnapshotMatch, cacheState,
                                                 simulationResult);
  }

  if (!this->iteratorState->IsFinished())
    this->NormalizeSnapshotManager(lastNWarp, lastSnapshotMatch);
}

void WarpingManager::NormalizeSnapshotManager(
    size_t lastNWarp, const std::shared_ptr<SnapshotMatch> &lastSnapshotMatch) {
  const auto snapshotItDiff = this->iteratorState->GetCurrentValue() -
                              lastSnapshotMatch->GetNewerItVal();
  if (snapshotItDiff != 0) {
    assert(lastNWarp != 0);
    assert(snapshotItDiff ==
           (long)lastNWarp * lastSnapshotMatch->GetItValDiff());
    lastSnapshotMatch->WarpOnlyNewerSnapshot(lastNWarp);
  }
  //
  // Not deleting old snapshots slows down the simulation!
  //
  // This happens because the future snapshots get matched with these old ones
  // ones but the checks for warping usually fail because of the large gap
  // between the snapshots.
  //
  this->snapshotManager.DeleteSnapshots();
  this->snapshotManager.AddSnapshot(lastSnapshotMatch->GetNewerSnapshot());
}

void WarpingManager::InitialWarp(const WarpInfo &warpInfo,
                                 CacheState &cacheState,
                                 SimulationResult &simulationResult) {
  this->OnSuccessfulInitialWarp(warpInfo);
  this->Warp(warpInfo, cacheState, simulationResult);
}

size_t WarpingManager::ComputeNWarp(const SnapshotMatch &snapshotMatch) {
  const auto nMatch = snapshotMatch.GetNIterationsBetween();
  const auto nIterationsToWarp = this->ComputeIterationsToWarp(snapshotMatch);
  if (nIterationsToWarp < nMatch)
    return 0;
  return nIterationsToWarp / nMatch;
}

size_t
WarpingManager::ComputeIterationsToWarp(const SnapshotMatch &snapshotMatch) {
  return this->furthestWarpingPointManager.ComputeIterationsToWarp(
      snapshotMatch);
}

void WarpingManager::Warp(const WarpInfo &warpInfo, CacheState &cacheState,
                          SimulationResult &simulationResult) {
  this->OnSuccessfulWarp(warpInfo);
  cacheState.Warp(warpInfo);
  this->iteratorState->Warp(warpInfo);
  simulationResult.Warp(warpInfo);
}

WarpInfo
WarpingManager::CreateWarpInfo(size_t nWarp,
                               const SnapshotMatch &snapshotMatch) const {
  assert(nWarp >= 1);
  return {snapshotMatch.GetIteratorId(),
          snapshotMatch.GetNIterationsBetween() * nWarp,
          snapshotMatch.GetItValDiff() * (long)nWarp,
          snapshotMatch.GetHitMissCountDiff() * nWarp,
          snapshotMatch.GetNRightRotationsBetweenByCacheLevel() * nWarp};
}

void WarpingManager::OnIteratorFinish() {
  this->snapshotManager.DeleteSnapshots();
}
