#pragma once

#include <list>
#include <string>

#include "CacheState/CacheState.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Settings/WarpingSettings.hpp"
#include "Simulation/NodeId.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Simulation/SimulationResult.hpp"
#include "Util/Util.hpp"

class SimulationNode {

public:
  explicit SimulationNode(NodeId nodeId);

  SimulationNode(NodeId nodeId, const isl::set &iterationDomain);

  virtual ~SimulationNode() = default;

  virtual void Simulate(CacheState &cacheState,
                        SimulationResult &simulationResult) = 0;

  [[nodiscard]] virtual std::vector<std::shared_ptr<SimulationAccessInfo>>
  GetSimulationAccessInfos() const = 0;

  [[nodiscard]] const NodeId &GetNodeId() const;

  [[nodiscard]] const isl::set &GetIterationDomain() const;

protected:
  NodeId nodeId;
  isl::set iterationDomain;
};
