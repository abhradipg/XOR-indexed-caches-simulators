#pragma once

#include <isl/cpp.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Address/SymbolicAddress.hpp"
#include "CacheState/CacheState.hpp"
#include "ProgramInfo/ProgramInfo.hpp"
#include "ProgramInfo/StatementInfo.hpp"
#include "Simulation/AccessSimulationNode.hpp"
#include "Simulation/IterationSimulationNode.hpp"
#include "Simulation/NodeId.hpp"
#include "Simulation/SimulationNode.hpp"
#include "Simulation/SimulationTreeBuilder.hpp"
#include "Util/IslUtil.hpp"
#include "Util/StrUtil.hpp"
#include "Util/Util.hpp"

class SimulationTree {
public:
  SimulationTree(const ProgramInfo &pi,
                 const SimulationSettings &simulationSettings);

  SimulationResult Simulate(CacheState &cacheState);

  [[nodiscard]] const GlobalIteratorState &GetGlobalIteratorState() const;

private:
  explicit SimulationTree(const SimulationTreeBuilder &simulationTreeBuilder);

  const std::vector<std::shared_ptr<SimulationNode>> rootNodes;

  const GlobalIteratorState globalIteratorState;
};
