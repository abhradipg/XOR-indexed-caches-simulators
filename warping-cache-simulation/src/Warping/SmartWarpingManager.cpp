#include "SmartWarpingManager.hpp"

SmartWarpingManager::SmartWarpingManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : WarpingManager(simulationAccessInfos, iteratorStateMap),
      smartWarpingPeriod(this->warpingPeriod) {
  assert(this->lastInitialWarpTryNEmptyCacheLines != 0);
}

void SmartWarpingManager::OnIteratorFinish() {
  this->ResetSnapshots();
  // this->smartWarpingPeriod = this->warpingPeriod;
}

bool SmartWarpingManager::ShouldTryWarping(const CacheState &cacheState) const {
  return this->IsItTimeToWarpAccordingToSmartPeriod() &&
         this->IsItSmartToWarpAccordingToCacheState(cacheState);
}

void SmartWarpingManager::OnInitialWarpTry(const CacheState &cacheState) {
  this->lastInitialWarpTryNEmptyCacheLines = cacheState.GetNumberOfEmptyLines();
}

void SmartWarpingManager::OnSuccessfulInitialWarp(const WarpInfo &) {
  this->smartWarpingPeriod = this->warpingPeriod;
}

void SmartWarpingManager::OnSuccessfulWarp(const WarpInfo &) {
  this->smartWarpingPeriod = this->warpingPeriod;
}

void SmartWarpingManager::OnUnsuccessfulInitialWarp() {
  this->smartWarpingPeriod += this->warpingPeriod;
}

bool SmartWarpingManager::IsItTimeToWarpAccordingToSmartPeriod() const {
  const auto nthIteration = this->iteratorState->GetNthIteration();
  return (nthIteration - 1) % this->smartWarpingPeriod == 0;
}

bool SmartWarpingManager::IsItSmartToWarpAccordingToCacheState(
    const CacheState &cacheState) const {
  const auto nEmptyCacheLines = cacheState.GetNumberOfEmptyLines();
  return nEmptyCacheLines == 0 ||
         nEmptyCacheLines == this->lastInitialWarpTryNEmptyCacheLines;
}
