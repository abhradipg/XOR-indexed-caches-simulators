#include "ConditionalConflictManager.hpp"

ConditionalConflictManager::ConditionalConflictManager(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap) {
  this->InitCheckers(simulationAccessInfos);
}

bool ConditionalConflictManager::HasDependingConditions() const {
  return !this->checkers.empty();
}

size_t ConditionalConflictManager::ComputeIterationsToWarp(
    const SnapshotMatch &sm) const {
  assert(this->HasDependingConditions());
  assert(sm.GetIteratorId() == this->iteratorStateMap.GetChangingIteratorId());
  auto nMatch = sm.GetNIterationsBetween();

  auto nIterations = std::numeric_limits<size_t>::max();
  for (const auto &checker : this->checkers) {
    nIterations =
        std::min(nIterations, checker.ComputeIterationsConditionComplies(sm));
    if (nIterations == 0)
      return 0;
    if (nIterations < nMatch)
      return 0;
  }

  assert(nIterations != std::numeric_limits<size_t>::max());
  return nIterations;
}

void ConditionalConflictManager::InitCheckers(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos) {
  const auto itId = this->iteratorStateMap.GetChangingIteratorId();
  for (const auto &sai : simulationAccessInfos) {
    if (!sai->IsConditional())
      continue;
    const auto &sci = sai->GetSimulationConditionInfo();
    if (sci.DoesConditionDependOnIterator(itId))
      this->checkers.emplace_back(sci.GetConditionIteratorIds(),
                                  sci.GetConditionDomain(),
                                  sci.GetIteratorStateMap());
  }
}
