#pragma once

#include <isl/cpp.h>
#include <list>
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
#include "Util/IslUtil.hpp"
#include "Util/StrUtil.hpp"
#include "Util/Util.hpp"

class SimulationTreeBuilder {
public:
  SimulationTreeBuilder(const ProgramInfo &pi,
                        const SimulationSettings &simulationSettings);

  [[nodiscard]] std::vector<std::shared_ptr<SimulationNode>>
  GetSortedRootNodes() const;

  [[nodiscard]] GlobalIteratorState ExtractGlobalIteratorState() const;

private:
  size_t timestampDim;

  std::map<NodeId, isl::set> iterationDomains;

  std::unordered_map<NodeId, size_t> iteratorIds;

  std::vector<std::string> iteratorUniqueNames;

  std::unordered_map<NodeId, std::shared_ptr<IterationSimulationNode>>
      iterationNodes;

  std::map<NodeId, std::shared_ptr<SimulationNode>> rootNodes;

  void BuildSimulationTree(const ProgramInfo &pi,
                           const SimulationSettings &simulationSettings);

  void InitTimestampDim(const ProgramInfo &pi);

  void InitIterationDomains(const ProgramInfo &pi);

  void InitIteratorIds();

  void InitIteratorUniqueNames();

  void InitIterationSimulators(const WarpingSettings &warpingSettings);

  void MakeIterationDomainsIteratorNamesUnique();

  void AddIterationNodes();

  void AddAccessNodes(const ProgramInfo &pi);

  void ExtendIterationDomain(size_t dim, const StatementInfo &si);

  void AddIterationNode(const NodeId &nid, const isl::set &itDom);

  void AddIterationNodeToParent(const NodeId &nid, const NodeId &parentNid,
                                const isl::set &itDom);

  void AddIterationNodeToRoot(const NodeId &nid, const isl::set &itDom);

  void AddAccessNode(const StatementInfo &si);

  void AddAccessNodeToParent(const NodeId &nid, const NodeId &parentNid,
                             const StatementInfo &si);

  void AddAccessNodeToRoot(const NodeId &nid, const StatementInfo &si);

  [[nodiscard]] size_t GetIteratorId(const NodeId &nid) const;

  [[nodiscard]] const std::string &
  GetIteratorUniqueName(const NodeId &nid) const;

  [[nodiscard]] const std::string &GetIteratorName(const NodeId &nid) const;

  [[nodiscard]] std::shared_ptr<AccessSimulationNode>
  CreateAccessSimulationNode(const NodeId &nid, const isl::set &itDomSet,
                             const StatementInfo &si,
                             const IteratorStateMap &parentIsm) const;

  [[nodiscard]] std::shared_ptr<IterationSimulationNode>
  CreateIterationSimulationNode(const NodeId &nid, const isl::set &itDom,
                                const IteratorStateMap &parentIsm) const;

  [[nodiscard]] std::shared_ptr<IteratorState>
  CreateIteratorState(const NodeId &nid, const isl::set &itDom,
                      IteratorStateMap &ism) const;

  [[nodiscard]] IteratorInfo CreateIteratorInfo(const NodeId &nid,
                                                long stride) const;

  [[nodiscard]] IteratorBound
  CreateIteratorBound(const NodeId &nid, const isl::set &itDom,
                      const IteratorStateMap &ism) const;
};
