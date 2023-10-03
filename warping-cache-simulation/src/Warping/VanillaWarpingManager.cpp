#include "VanillaWarpingManager.hpp"

VanillaWarpingManager::VanillaWarpingManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : WarpingManager(simulationAccessInfos, iteratorStateMap) {}

void VanillaWarpingManager::OnIteratorFinish() { this->ResetSnapshots(); }

bool VanillaWarpingManager::ShouldTryWarping(const CacheState &) const {
  return this->IsItTimeToWarpAccordingToPeriod();
}

void VanillaWarpingManager::OnInitialWarpTry(const CacheState &) {}

void VanillaWarpingManager::OnSuccessfulInitialWarp(const WarpInfo &) {}

void VanillaWarpingManager::OnSuccessfulWarp(const WarpInfo &) {}

void VanillaWarpingManager::OnUnsuccessfulInitialWarp() {}
