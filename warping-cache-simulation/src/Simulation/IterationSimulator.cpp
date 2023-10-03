#include "IterationSimulator.hpp"

IterationSimulator::IterationSimulator(
    IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
    const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes)
    : iteratorState(iteratorState), iteratorStateMap(iteratorStateMap),
      nodes(nodes) {}

void IterationSimulator::SimulateNodes(CacheState &cacheState,
                                       SimulationResult &simulationResult) {
  for (auto &el : this->nodes)
    el.second->Simulate(cacheState, simulationResult);
}