#include "SimulationTree.hpp"

SimulationTree::SimulationTree(const ProgramInfo &pi,
                               const SimulationSettings &simulationSettings)
    : SimulationTree(SimulationTreeBuilder(pi, simulationSettings)) {}

SimulationTree::SimulationTree(
    const SimulationTreeBuilder &simulationTreeBuilder)
    : rootNodes(simulationTreeBuilder.GetSortedRootNodes()),
      globalIteratorState(simulationTreeBuilder.ExtractGlobalIteratorState()) {}

SimulationResult SimulationTree::Simulate(CacheState &cacheState) {
  SimulationResult simulationResult(cacheState.GetNumberOfCacheLevels());
  for (const auto &node : this->rootNodes)
    node->Simulate(cacheState, simulationResult);
  return simulationResult;
}

const GlobalIteratorState &SimulationTree::GetGlobalIteratorState() const {
  return this->globalIteratorState;
}
