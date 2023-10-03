#pragma once

#include <isl/cpp.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "IteratorState/IteratorState.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/AccessSimulationNode.hpp"
#include "Simulation/IterationSimulator.hpp"
#include "Simulation/NonWarpingIterationSimulator.hpp"
#include "Simulation/SimulationNode.hpp"
#include "Simulation/WarpingIterationSimulator.hpp"
#include "Warping/SmartWarpingManager.hpp"
#include "Warping/VanillaWarpingManager.hpp"
#include "Warping/WarpingManager.hpp"

class IterationSimulationNode : public SimulationNode {
public:
  IterationSimulationNode(const NodeId &nodeId, const isl::set &iterationDomain,
                          IteratorStateMap iteratorStateMap);

  void Simulate(CacheState &cacheState,
                SimulationResult &simulationResult) override;

  [[nodiscard]] std::vector<std::shared_ptr<SimulationAccessInfo>>
  GetSimulationAccessInfos() const override;

  [[nodiscard]] const IteratorStateMap &GetIteratorStateMap() const;

  [[nodiscard]] const std::shared_ptr<IteratorState> &GetIteratorState() const;

  void AddChild(const std::shared_ptr<SimulationNode> &child);

  void InitIterationSimulator(const WarpingSettings &warpingSettings);

private:
  std::map<NodeId, std::shared_ptr<SimulationNode>> children;

  IteratorStateMap iteratorStateMap;
  std::shared_ptr<IteratorState> iteratorState;

  std::unique_ptr<IterationSimulator> iterationSimulator;

  [[nodiscard]] std::unique_ptr<WarpingManager>
  CreateWarpingManager(const WarpingSettings &warpingSettings) const;

  [[nodiscard]] std::unique_ptr<WarpingIterationSimulator>
  CreateWarpingIterationSimulator(const WarpingSettings &warpingSettings) const;

  [[nodiscard]] std::unique_ptr<NonWarpingIterationSimulator>
  CreateNonWarpingIterationSimulator() const;
};
