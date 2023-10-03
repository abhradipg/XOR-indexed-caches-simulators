#include "WarpingPeriodCalculator.hpp"

WarpingPeriodCalculator::WarpingPeriodCalculator(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorState &iteratorState)
    : warpingPeriod(this->ComputeWarpingPeriod(simulationAccessInfos,
                                               iteratorState.GetIteratorId())) {
}

size_t WarpingPeriodCalculator::GetWarpingPeriod() {
  return this->warpingPeriod;
}

size_t WarpingPeriodCalculator::ComputeWarpingPeriod(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    size_t itId) const {
  size_t period = 1;
  for (const auto &sai : simulationAccessInfos)
    period = std::lcm(period, sai->GetDifferentMemoryBlockPeriod(itId));
  return period;
}
