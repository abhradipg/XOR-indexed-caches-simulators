#pragma once

#include "Warping/WarpingManager.hpp"
#include <numeric>

class SmartWarpingManager : public WarpingManager {
public:
  SmartWarpingManager(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                          &simulationAccessInfos,
                      const IteratorStateMap &iteratorStateMap);

  void OnIteratorFinish() override;

private:
  size_t smartWarpingPeriod;

  size_t lastInitialWarpTryNEmptyCacheLines = GlobalVars::nTotalCacheLines;

  [[nodiscard]] bool
  ShouldTryWarping(const CacheState &cacheState) const override;

  void OnInitialWarpTry(const CacheState &cacheState) override;

  void OnSuccessfulInitialWarp(const WarpInfo &warpInfo) override;

  void OnSuccessfulWarp(const WarpInfo &warpInfo) override;

  void OnUnsuccessfulInitialWarp() override;

  [[nodiscard]] bool IsItTimeToWarpAccordingToSmartPeriod() const;

  [[nodiscard]] bool
  IsItSmartToWarpAccordingToCacheState(const CacheState &cacheState) const;
};
