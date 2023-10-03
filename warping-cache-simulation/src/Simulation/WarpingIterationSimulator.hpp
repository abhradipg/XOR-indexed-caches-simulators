#pragma once

#include "Simulation/IterationSimulator.hpp"
#include "Warping/WarpingManager.hpp"

class WarpingIterationSimulator : public IterationSimulator {
public:
  WarpingIterationSimulator(
      std::unique_ptr<WarpingManager> warpingManager,
      IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
      const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes);

  void Simulate(CacheState &cacheState,
                SimulationResult &simulationResult) override;

private:
  std::unique_ptr<WarpingManager> warpingManager;
};
