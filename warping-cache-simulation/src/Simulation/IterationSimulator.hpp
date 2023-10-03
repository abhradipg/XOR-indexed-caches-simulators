#pragma once

#include <map>
#include <memory>

#include "CacheState/CacheState.hpp"
#include "IteratorState/IteratorState.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/NodeId.hpp"
#include "Simulation/SimulationNode.hpp"

class IterationSimulator {
public:
  IterationSimulator(
      IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
      const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes);

  virtual ~IterationSimulator() = default;

  virtual void Simulate(CacheState &cacheState,
                        SimulationResult &simulationResult) = 0;

protected:
  IteratorState &iteratorState;
  const IteratorStateMap &iteratorStateMap;
  const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes;

  void SimulateNodes(CacheState &cacheState,
                     SimulationResult &simulationResult);
};
