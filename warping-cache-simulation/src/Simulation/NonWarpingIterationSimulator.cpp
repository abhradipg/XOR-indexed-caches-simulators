#include "NonWarpingIterationSimulator.hpp"

NonWarpingIterationSimulator::NonWarpingIterationSimulator(
    IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
    const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes)
    : IterationSimulator(iteratorState, iteratorStateMap, nodes) {}

void NonWarpingIterationSimulator::Simulate(
    CacheState &cacheState, SimulationResult &simulationResult) {
  this->iteratorState.Refresh(this->iteratorStateMap);
  while (!this->iteratorState.IsFinished()) {
    this->SimulateNodes(cacheState, simulationResult);
    this->iteratorState.Advance();
  }
}