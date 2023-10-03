#pragma once

#include "Warping/WarpingManager.hpp"

class VanillaWarpingManager : public WarpingManager {
public:
  VanillaWarpingManager(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                            &simulationAccessInfos,
                        const IteratorStateMap &iteratorStateMap);

  void OnIteratorFinish() override;

private:
  [[nodiscard]] bool ShouldTryWarping(const CacheState &) const override;

  void OnInitialWarpTry(const CacheState &) override;

  void OnSuccessfulInitialWarp(const WarpInfo &) override;

  void OnSuccessfulWarp(const WarpInfo &) override;

  void OnUnsuccessfulInitialWarp() override;
};
