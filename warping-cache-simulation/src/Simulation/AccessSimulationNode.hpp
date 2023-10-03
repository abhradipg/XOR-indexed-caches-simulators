#pragma once

#include <memory>

#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Simulation/SimulationNode.hpp"

class AccessSimulationNode : public SimulationNode {
public:
  AccessSimulationNode(const NodeId &id, const isl::set &iterationDomain,
                       const std::vector<AccessInfo> &accessInfos,
                       const IteratorStateMap &iteratorStateMap);

  void Simulate(CacheState &cacheState,
                SimulationResult &simulationResult) override;

  [[nodiscard]] std::vector<std::shared_ptr<SimulationAccessInfo>>
  GetSimulationAccessInfos() const override;

private:
  const IteratorStateMap &iteratorStateMap;
  std::vector<std::shared_ptr<SimulationAccessInfo>> simulationAccessInfos;

  void InitSimulationAccessInfos(const std::vector<AccessInfo> &accessInfos);

  [[nodiscard]] std::vector<CacheAccess> GetCacheAccessesToSimulate() const;
};
