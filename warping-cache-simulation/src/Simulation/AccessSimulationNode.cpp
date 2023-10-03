#include "AccessSimulationNode.hpp"

AccessSimulationNode::AccessSimulationNode(
    const NodeId &nodeId, const isl::set &iterationDomain,
    const std::vector<AccessInfo> &accessInfos,
    const IteratorStateMap &iteratorStateMap)
    : SimulationNode(nodeId, iterationDomain),
      iteratorStateMap(iteratorStateMap) {
  this->InitSimulationAccessInfos(accessInfos);
}

void AccessSimulationNode::Simulate(CacheState &cacheState,
                                    SimulationResult &simulationResult) {
  const auto &toSimulate = this->GetCacheAccessesToSimulate();

  if (toSimulate.empty())
    return;

  cacheState.ProcessAccesses(toSimulate, simulationResult);
}

std::vector<std::shared_ptr<SimulationAccessInfo>>
AccessSimulationNode::GetSimulationAccessInfos() const {
  return this->simulationAccessInfos;
}

void AccessSimulationNode::InitSimulationAccessInfos(
    const std::vector<AccessInfo> &accessInfos) {
  for (const auto &ai : accessInfos) {
    this->simulationAccessInfos.push_back(
        std::make_shared<SimulationAccessInfo>(ai, this->iterationDomain,
                                               this->iteratorStateMap));
  }
}

std::vector<CacheAccess>
AccessSimulationNode::GetCacheAccessesToSimulate() const {
  std::vector<CacheAccess> toSimulate;
  toSimulate.reserve(this->simulationAccessInfos.size());
  for (const auto &sai : this->simulationAccessInfos)
    if (sai->ShouldSimulate())
      toSimulate.emplace_back(sai->IsRead(), sai->GetCurrentAddress());
  return toSimulate;
}