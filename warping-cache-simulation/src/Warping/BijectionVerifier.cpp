#include "BijectionVerifier.hpp"

BijectionVerifier::BijectionVerifier(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap) {
  this->InitCheckers(simulationAccessInfos);
  this->InitCoefficients();
}

bool BijectionVerifier::ShouldCheckCacheBijection(
    const SnapshotMatch &sm) const {
  const auto &old = sm.GetOlderSnapshot();
  if (old->DoesHaveSingleCoefficient() && this->coefficients.size() == 1)
    return old->GetSingleCoefficient() != *this->coefficients.begin();

  return true;
}

bool BijectionVerifier::DoesCacheBijectionComplyWithAccessBijection(
    long furthestItVal, const SnapshotMatch &sm) {
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    if (!this->DoesCacheBijectionComplyWithAccessBijection(furthestItVal, sm,
                                                           i))
      return false;

  return true;
}

bool BijectionVerifier::DoesCacheBijectionComplyWithAccessBijection(
    long furthestItVal, const SnapshotMatch &sm, size_t cacheLevel) {
  const auto itId = sm.GetIteratorId();
  this->RefreshCheckers(furthestItVal, sm);
  const auto &oldLines = sm.GetOldLines(cacheLevel);
  const auto &newLines = sm.GetNewLines(cacheLevel);
  assert(oldLines.size() == newLines.size());
  const auto nLines = oldLines.size();
  for (size_t i = 0; i < nLines; i++) {
    if (!this->IsMappingOkay(itId, oldLines[i].GetLastAccessedAddress(),
                             newLines[i].GetLastAccessedAddress()))
      return false;
  }
  return true;
}

bool BijectionVerifier::IsMappingOkay(size_t itId, const Address &oldAddr,
                                      const Address &newAddr) {
  assert(itId == this->iteratorStateMap.GetChangingIteratorId());
  const auto coefficient = oldAddr.GetSymbolicAddress()->GetCoefficient(itId);
  const auto oldMb = Util::ComputeMemoryBlock(oldAddr.GetConcreteAddress());
  const auto newMb = Util::ComputeMemoryBlock(newAddr.GetConcreteAddress());
  for (auto &el : this->checkersByCoefficient) {
    const auto shouldCheck = el.first != coefficient;
    if (shouldCheck &&
        !this->IsMappingOkayAccordingToCheckers(oldMb, newMb, el.second))
      return false;
  }
  return true;
}

bool BijectionVerifier::IsMappingOkayAccordingToCheckers(
    size_t oldMb, size_t newMb,
    std::vector<MemoryBlockMappingChecker> &checkers) const {
  for (auto &checker : checkers)
    if (!checker.IsOkay(oldMb, newMb))
      return false;
  return true;
}

void BijectionVerifier::InitCheckers(
    const std::vector<std::shared_ptr<SimulationAccessInfo>>
        &simulationAccessInfos) {
  const auto &itId = this->iteratorStateMap.GetChangingIteratorId();
  std::unordered_set<std::string> addedSymStr;
  for (const auto &sai : simulationAccessInfos) {
    const auto &sa = sai->GetSymbolicAddress();
    const auto &saStr =
        sa->ToString() + "_" + IslUtil::ToString(sai->GetAccessDomain());
    if (addedSymStr.count(saStr) == 0) {
      addedSymStr.insert(saStr);
      const auto c = sa->GetCoefficient(itId);
      this->checkersByCoefficient[c].emplace_back(*sai, this->iteratorStateMap);
    }
  }
}

void BijectionVerifier::InitCoefficients() {
  for (const auto &el : this->checkersByCoefficient)
    this->coefficients.insert(el.first);
}

void BijectionVerifier::RefreshCheckers(long furthestItVal,
                                        const SnapshotMatch &sm) {
  for (auto &el : this->checkersByCoefficient)
    for (auto &checker : el.second)
      checker.Refresh(furthestItVal, sm);
}
