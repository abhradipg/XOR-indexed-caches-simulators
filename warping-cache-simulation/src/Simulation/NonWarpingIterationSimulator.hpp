#pragma once

#include "Simulation/IterationSimulator.hpp"

class NonWarpingIterationSimulator : public IterationSimulator {
public:
  NonWarpingIterationSimulator(
      IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
      const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes);

  void Simulate(CacheState &cacheState,
                SimulationResult &simulationResult) override;
};
