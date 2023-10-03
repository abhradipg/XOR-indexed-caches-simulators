#include "WarpingIterationSimulator.hpp"

WarpingIterationSimulator::WarpingIterationSimulator(
    std::unique_ptr<WarpingManager> warpingManager,
    IteratorState &iteratorState, const IteratorStateMap &iteratorStateMap,
    const std::map<NodeId, std::shared_ptr<SimulationNode>> &nodes)
    : IterationSimulator(iteratorState, iteratorStateMap, nodes),
      warpingManager(std::move(warpingManager)) {}

void WarpingIterationSimulator::Simulate(CacheState &cacheState,
                                         SimulationResult &simulationResult) {
  assert(this->warpingManager != nullptr);
  this->iteratorState.Refresh(this->iteratorStateMap);
  while (!this->iteratorState.IsFinished()) {
    this->warpingManager->TryWarping(cacheState, simulationResult);
    const auto couldWarpToTheEnd = this->iteratorState.IsFinished();
    if (!couldWarpToTheEnd) {
      this->SimulateNodes(cacheState, simulationResult);
      this->iteratorState.Advance();
    }
  }
  this->warpingManager->OnIteratorFinish();
}
