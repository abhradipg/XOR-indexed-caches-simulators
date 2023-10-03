#include "IterationSimulationNode.hpp"

IterationSimulationNode::IterationSimulationNode(
    const NodeId &nodeId, const isl::set &iterationDomain,
    IteratorStateMap iteratorStateMap)
    : SimulationNode(nodeId, iterationDomain),
      iteratorStateMap(std::move(iteratorStateMap)) {
  assert(this->nodeId.IsIterationNodeId());
  this->iteratorState = this->iteratorStateMap.GetChangingIteratorState();
  assert(this->iteratorState->GetIteratorName() ==
         this->nodeId.GetIteratorName());
}

void IterationSimulationNode::Simulate(CacheState &cacheState,
                                       SimulationResult &simulationResult) {
  this->iterationSimulator->Simulate(cacheState, simulationResult);
}

std::vector<std::shared_ptr<SimulationAccessInfo>>
IterationSimulationNode::GetSimulationAccessInfos() const {
  std::vector<std::shared_ptr<SimulationAccessInfo>> res;
  for (const auto &el : this->children) {
    const auto &sais = el.second->GetSimulationAccessInfos();
    res.insert(res.end(), sais.begin(), sais.end());
  }
  return res;
}

const IteratorStateMap &IterationSimulationNode::GetIteratorStateMap() const {
  return this->iteratorStateMap;
}

const std::shared_ptr<IteratorState> &
IterationSimulationNode::GetIteratorState() const {
  return this->iteratorState;
}

void IterationSimulationNode::AddChild(
    const std::shared_ptr<SimulationNode> &child) {
  const auto &nid = child->GetNodeId();
  assert(this->children.count(nid) == 0);
  this->children.emplace(nid, child);
}

void IterationSimulationNode::InitIterationSimulator(
    const WarpingSettings &warpingSettings) {
  if (warpingSettings.tryWarping && this->iteratorState->IsWarpingApplicable())
    this->iterationSimulator =
        this->CreateWarpingIterationSimulator(warpingSettings);
  else
    this->iterationSimulator = this->CreateNonWarpingIterationSimulator();
}

std::unique_ptr<WarpingManager> IterationSimulationNode::CreateWarpingManager(
    const WarpingSettings &warpingSettings) const {
  if (warpingSettings.useHeuristics)
    return std::make_unique<SmartWarpingManager>(
        this->GetSimulationAccessInfos(), this->iteratorStateMap);
  else
    return std::make_unique<VanillaWarpingManager>(
        this->GetSimulationAccessInfos(), this->iteratorStateMap);
}

std::unique_ptr<WarpingIterationSimulator>
IterationSimulationNode::CreateWarpingIterationSimulator(
    const WarpingSettings &warpingSettings) const {
  return std::make_unique<WarpingIterationSimulator>(
      this->CreateWarpingManager(warpingSettings), *this->iteratorState,
      this->iteratorStateMap, this->children);
}

std::unique_ptr<NonWarpingIterationSimulator>
IterationSimulationNode::CreateNonWarpingIterationSimulator() const {
  return std::make_unique<NonWarpingIterationSimulator>(
      *this->iteratorState, this->iteratorStateMap, this->children);
}
