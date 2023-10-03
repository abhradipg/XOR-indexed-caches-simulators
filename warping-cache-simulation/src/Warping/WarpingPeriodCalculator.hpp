#pragma once

#include <numeric>
#include <vector>

#include "IteratorState/IteratorState.hpp"
#include "Simulation/SimulationAccessInfo.hpp"

class WarpingPeriodCalculator {
public:
  WarpingPeriodCalculator(
      const std::vector<std::shared_ptr<SimulationAccessInfo>>
          &simulationAccessInfos,
      const IteratorState &iteratorState);

  [[nodiscard]] size_t GetWarpingPeriod();

private:
  // 1 means try warping every iteration, 2 means every two iterations...
  size_t warpingPeriod;

  [[nodiscard]] size_t
  ComputeWarpingPeriod(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                           &simulationAccessInfos,
                       size_t itId) const;
};
