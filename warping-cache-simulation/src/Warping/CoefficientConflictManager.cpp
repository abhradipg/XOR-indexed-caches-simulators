#include "CoefficientConflictManager.hpp"

CoefficientConflictManager::CoefficientConflictManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap) {
  this->InitConflicts(this->FilterDuplicateAccesses(simulationAccessInfos));
}

bool CoefficientConflictManager::HasCoefficientConflicts() const {
  return !this->conflicts.empty();
}

size_t CoefficientConflictManager::ComputeIterationsToWarp(
    const SnapshotMatch &snapshotMatch) const {
  assert(snapshotMatch.GetIteratorId() ==
         this->iteratorStateMap.GetChangingIteratorId());
  assert(this->HasCoefficientConflicts());
  auto nMatch = snapshotMatch.GetNIterationsBetween();

  auto nIterations = std::numeric_limits<size_t>::max();
  for (const auto &conflict : this->conflicts) {
    nIterations =
        std::min(nIterations,
                 conflict.ComputeIterationsUntilNextConflict(snapshotMatch));
    if (nIterations == 0)
      return 0;
    if (nIterations < nMatch)
      return 0;
  }

  assert(nIterations != std::numeric_limits<size_t>::max());
  return nIterations;
}

void CoefficientConflictManager::InitConflicts(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos) {
  const auto &byCoefficient =
      this->ExtractSimulationAccessInfosByCoefficient(simulationAccessInfos);
  if (byCoefficient.empty() || byCoefficient.size() == 1)
    return;

  for (auto it1 = byCoefficient.begin(); it1 != byCoefficient.end(); it1++)
    for (auto it2 = std::next(it1); it2 != byCoefficient.end(); it2++)
      this->AddConflictsForDiffCoefficientAccesses(it1->second, it2->second);
}

std::vector<std::shared_ptr<SimulationAccessInfo>>
CoefficientConflictManager::FilterDuplicateAccesses(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos) const {
  std::vector<std::shared_ptr<SimulationAccessInfo>> filtered;
  std::set<std::string> addedSymAddrStrings;
  for (const auto &sai : simulationAccessInfos) {
    const auto &sa = sai->GetSymbolicAddress();
    const auto &saStr =
        sa->ToString() + IslUtil::ToString(sai->GetAccessDomain());
    if (addedSymAddrStrings.count(saStr) == 0) {
      filtered.push_back(sai);
      addedSymAddrStrings.insert(saStr);
    }
  }
  return filtered;
}

std::unordered_map<long, std::vector<std::shared_ptr<SimulationAccessInfo>>>
CoefficientConflictManager::ExtractSimulationAccessInfosByCoefficient(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos) const {
  const auto itId = this->iteratorStateMap.GetChangingIteratorId();
  std::unordered_map<long, std::vector<std::shared_ptr<SimulationAccessInfo>>>
      byCoefficient;
  for (const auto &sai : simulationAccessInfos) {
    const auto coefficient = sai->GetSymbolicAddress()->GetCoefficient(itId);
    byCoefficient[coefficient].push_back(sai);
  }
  return byCoefficient;
}

void CoefficientConflictManager::AddConflictsForDiffCoefficientAccesses(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos1,
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos2) {
  assert(!simulationAccessInfos1.empty() && !simulationAccessInfos2.empty());
  for (const auto &sai1 : simulationAccessInfos1) {
    for (const auto &sai2 : simulationAccessInfos2) {
      if (sai1->IsAccessedArrayMemoryBlockIntervalDisjoint(*sai2))
        continue;
      CoefficientConflict conflict(*sai1, *sai2, this->iteratorStateMap);
      if (!conflict.IsEmpty())
        this->conflicts.push_back(conflict);
    }
  }
}
