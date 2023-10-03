#include "SimulationTreeBuilder.hpp"

SimulationTreeBuilder::SimulationTreeBuilder(
    const ProgramInfo &pi, const SimulationSettings &simulationSettings) {
  this->InitTimestampDim(pi);
  this->InitIterationDomains(pi);
  this->InitIteratorIds();
  this->InitIteratorUniqueNames();
  this->MakeIterationDomainsIteratorNamesUnique();
  this->BuildSimulationTree(pi, simulationSettings);
}

std::vector<std::shared_ptr<SimulationNode>>
SimulationTreeBuilder::GetSortedRootNodes() const {
  std::vector<std::shared_ptr<SimulationNode>> sortedRootNodes;
  for (const auto &el : this->rootNodes)
    sortedRootNodes.push_back(el.second);
  return sortedRootNodes;
}

GlobalIteratorState SimulationTreeBuilder::ExtractGlobalIteratorState() const {
  GlobalIteratorState globalIteratorState;
  globalIteratorState.Reserve(this->iteratorIds.size());
  for (const auto &el : this->iterationDomains) {
    const auto &nid = el.first;
    const auto itId = this->GetIteratorId(nid);
    assert(this->iterationNodes.count(nid) == 1);
    const auto &itState = this->iterationNodes.at(nid)->GetIteratorState();
    const auto &itUniqueName = itState->GetIteratorUniqueName();
    assert(itUniqueName == this->GetIteratorUniqueName(nid));
    globalIteratorState.AddIterator(itId, itUniqueName,
                                    itState->GetCurrentValuePointer());
  }
  return globalIteratorState;
}

void SimulationTreeBuilder::BuildSimulationTree(
    const ProgramInfo &pi, const SimulationSettings &simulationSettings) {
  this->AddIterationNodes();
  this->AddAccessNodes(pi);
  this->InitIterationSimulators(simulationSettings.warpingSettings);
}

void SimulationTreeBuilder::InitTimestampDim(const ProgramInfo &pi) {
  assert(!pi.GetStatementInfos().empty());
  this->timestampDim = IslUtil::GetSetDimension(
      pi.GetStatementInfos().begin()->second.GetTimestampDomain());
  assert(this->timestampDim >= 2);
}

void SimulationTreeBuilder::InitIterationDomains(const ProgramInfo &pi) {
  auto loopAccessStatementsExist = false;
  for (size_t dim = 0; dim < this->timestampDim; dim++)
    for (auto &el : pi.GetStatementInfos()) {
      const auto &timestampStrDims = el.second.GetTimestampStrDims();
      if (!StrUtil::IsInteger(timestampStrDims[dim])) {
        loopAccessStatementsExist = true;
        this->ExtendIterationDomain(dim, el.second);
      }
    }

  if (loopAccessStatementsExist)
    assert(!this->iterationDomains.empty());
  else
    assert(this->iterationDomains.empty());
}

void SimulationTreeBuilder::InitIteratorIds() {
  size_t itId = 0;
  for (const auto &el : this->iterationDomains) {
    this->iteratorIds.emplace(el.first, itId);
    itId++;
  }
}

void SimulationTreeBuilder::InitIteratorUniqueNames() {
  this->iteratorUniqueNames.resize(this->iteratorIds.size());
  for (const auto &el : this->iteratorIds) {
    const auto &nid = el.first;
    const auto itId = el.second;
    std::string itName = nid.GetIteratorName();
    itName.append("_").append(std::to_string(itId));
    this->iteratorUniqueNames[itId] = itName;
  }
}

void SimulationTreeBuilder::InitIterationSimulators(
    const WarpingSettings &warpingSettings) {
  for (auto &el : this->iterationNodes)
    el.second->InitIterationSimulator(warpingSettings);
}

void SimulationTreeBuilder::MakeIterationDomainsIteratorNamesUnique() {
  for (auto &el : this->iterationDomains) {
    std::unordered_map<std::string, std::string> oldToNewNames;
    auto nid = el.first;
    while (true) {
      const auto &itName = this->GetIteratorName(nid);
      const auto &itUniqueName = this->GetIteratorUniqueName(nid);
      assert(oldToNewNames.count(itName) == 0);
      oldToNewNames.emplace(itName, itUniqueName);
      if (nid.HasParent())
        nid = nid.GetClosestParentId();
      else
        break;
    }
    el.second = IslUtil::RenameSetDims(el.second, oldToNewNames);
  }
}

void SimulationTreeBuilder::ExtendIterationDomain(size_t dim,
                                                  const StatementInfo &si) {
  const auto &ts = IslUtil::EliminateDimsAfterDim(si.GetTimestampDomain(), dim);
  const auto &dom = IslUtil::DropDimsAfterDim(ts, dim);
  const auto &nid = NodeId(dim, si);
  auto it = this->iterationDomains.find(nid);
  if (it == this->iterationDomains.end())
    this->iterationDomains[nid] = dom;
  else
    it->second = it->second.unite(dom).coalesce();
}

void SimulationTreeBuilder::AddIterationNodes() {
  for (const auto &el : this->iterationDomains)
    this->AddIterationNode(el.first, el.second);
}

void SimulationTreeBuilder::AddAccessNodes(const ProgramInfo &pi) {
  for (const auto &el : pi.GetStatementInfos())
    this->AddAccessNode(el.second);
}

void SimulationTreeBuilder::AddIterationNode(const NodeId &nid,
                                             const isl::set &itDom) {
  assert(nid.IsIterationNodeId());
  assert(this->iterationNodes.count(nid) == 0);
  if (nid.HasParent())
    this->AddIterationNodeToParent(nid, nid.GetClosestParentId(), itDom);
  else
    this->AddIterationNodeToRoot(nid, itDom);
}

void SimulationTreeBuilder::AddIterationNodeToParent(const NodeId &nid,
                                                     const NodeId &parentNid,
                                                     const isl::set &itDom) {
  auto it = this->iterationNodes.find(parentNid);
  assert(it != this->iterationNodes.end());
  const auto &parentIsm = it->second->GetIteratorStateMap();
  const auto &newNode =
      this->CreateIterationSimulationNode(nid, itDom, parentIsm);
  it->second->AddChild(newNode);
  this->iterationNodes.emplace(nid, newNode);
}

void SimulationTreeBuilder::AddIterationNodeToRoot(const NodeId &nid,
                                                   const isl::set &itDom) {
  IteratorStateMap parentIsm;
  const auto &newNode =
      this->CreateIterationSimulationNode(nid, itDom, parentIsm);
  this->rootNodes.emplace(nid, newNode);
  this->iterationNodes.emplace(nid, newNode);
}

void SimulationTreeBuilder::AddAccessNode(const StatementInfo &si) {
  assert(si.GetTimestampStrDims().size() == this->timestampDim);
  assert(!si.GetAccessInfos().empty());
  NodeId nid(this->timestampDim - 1, si);
  if (nid.IsIterationNodeId())
    this->AddAccessNodeToParent(nid.CreateAccessNodeId(), nid, si);
  else if (nid.HasParent())
    this->AddAccessNodeToParent(nid, nid.GetClosestParentId(), si);
  else
    this->AddAccessNodeToRoot(nid, si);
}

void SimulationTreeBuilder::AddAccessNodeToParent(const NodeId &nid,
                                                  const NodeId &parentNid,
                                                  const StatementInfo &si) {
  auto it = this->iterationNodes.find(parentNid);
  assert(it != this->iterationNodes.end());
  const auto &newNode =
      this->CreateAccessSimulationNode(nid, it->second->GetIterationDomain(),
                                       si, it->second->GetIteratorStateMap());
  it->second->AddChild(newNode);
}

void SimulationTreeBuilder::AddAccessNodeToRoot(const NodeId &nid,
                                                const StatementInfo &si) {
  IteratorStateMap emptyIsm;
  isl::set emptyItDom =
      IslUtil::CreateEmptySet(si.GetTimestampDomain().space());
  const auto &newNode =
      this->CreateAccessSimulationNode(nid, emptyItDom, si, emptyIsm);
  this->rootNodes.emplace(nid, newNode);
}

size_t SimulationTreeBuilder::GetIteratorId(const NodeId &nid) const {
  assert(nid.IsIterationNodeId());
  assert(this->iteratorIds.count(nid) == 1);
  return this->iteratorIds.at(nid);
}

const std::string &
SimulationTreeBuilder::GetIteratorUniqueName(const NodeId &nid) const {
  assert(nid.IsIterationNodeId());
  const auto itId = this->GetIteratorId(nid);
  assert(itId < this->iteratorUniqueNames.size());
  assert(!this->iteratorUniqueNames[itId].empty());
  return this->iteratorUniqueNames[itId];
}

const std::string &
SimulationTreeBuilder::GetIteratorName(const NodeId &nid) const {
  assert(nid.IsIterationNodeId());
  return nid.GetIteratorName();
}

std::shared_ptr<AccessSimulationNode>
SimulationTreeBuilder::CreateAccessSimulationNode(
    const NodeId &nid, const isl::set &itDomSet, const StatementInfo &si,
    const IteratorStateMap &parentIsm) const {
  return std::make_shared<AccessSimulationNode>(nid, itDomSet,
                                                si.GetAccessInfos(), parentIsm);
}

std::shared_ptr<IterationSimulationNode>
SimulationTreeBuilder::CreateIterationSimulationNode(
    const NodeId &nid, const isl::set &itDom,
    const IteratorStateMap &parentIsm) const {
  assert(nid.IsIterationNodeId());
  IteratorStateMap ism = parentIsm;
  ism.AddNewChangingIterator(this->CreateIteratorState(nid, itDom, ism));
  return std::make_shared<IterationSimulationNode>(nid, itDom, ism);
}

std::shared_ptr<IteratorState> SimulationTreeBuilder::CreateIteratorState(
    const NodeId &nid, const isl::set &itDom, IteratorStateMap &ism) const {
  assert(nid.IsIterationNodeId());
  const auto &itUniqueName = this->GetIteratorUniqueName(nid);
  const auto stride = IslUtil::GetSetDimNameStride(itDom, itUniqueName);
  return std::make_shared<IteratorState>(
      this->CreateIteratorInfo(nid, stride),
      this->CreateIteratorBound(nid, itDom, ism), ism);
}

IteratorInfo SimulationTreeBuilder::CreateIteratorInfo(const NodeId &nid,
                                                       long stride) const {
  assert(this->iteratorIds.count(nid) == 1);
  const auto itId = this->GetIteratorId(nid);
  const auto &itName = this->GetIteratorName(nid);
  const auto &itUniqueName = this->GetIteratorUniqueName(nid);
  assert(itUniqueName != itName);
  return {itId, itName, itUniqueName, stride};
}

IteratorBound
SimulationTreeBuilder::CreateIteratorBound(const NodeId &nid,
                                           const isl::set &itDom,
                                           const IteratorStateMap &ism) const {
  const auto &itUniqueName = this->GetIteratorUniqueName(nid);
  return {itDom, itUniqueName, ism.GetIteratorUniqueNamesToIteratorIds()};
}
