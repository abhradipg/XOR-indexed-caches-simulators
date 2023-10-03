#include "HierarchicalCacheState.hpp"

HierarchicalCacheState::HierarchicalCacheState(
    const std::vector<CacheSettings> &cacheSettingsVec,
    const GlobalIteratorState &globalIteratorState)
    : CacheState(0, cacheSettingsVec.size()),
      cacheStates(
          this->CreateCacheStates(cacheSettingsVec, globalIteratorState)),
      nLines(this->ExtractNumberOfLines()) {}

CacheAccessResult
HierarchicalCacheState::ProcessAccess(const CacheAccess &access,
                                      SimulationResult &simulationResult) {
  this->ProcessAccessOnCacheLevelsRecursively(0, access, simulationResult);
  return CacheAccessResult::Discard();
}

void HierarchicalCacheState::ProcessAccessOnCacheLevelsRecursively(
    size_t cacheLevel, const CacheAccess &access,
    SimulationResult &simulationResult) {
  const auto &res =
      this->cacheStates[cacheLevel]->ProcessAccess(access, simulationResult);

  const auto nextCacheLevel = cacheLevel + 1;
  if (nextCacheLevel == this->cacheStates.size())
    return;

  if (res.IsHit())
    this->HandleWriteThroughCacheLevel(nextCacheLevel, access, res,
                                       simulationResult);
  else {
    this->HandleWriteBackOnCacheLevel(nextCacheLevel, res, simulationResult);
    this->ProcessAccessOnCacheLevelsRecursively(nextCacheLevel, access,
                                                simulationResult);
  }
}

void HierarchicalCacheState::HandleWriteThroughCacheLevel(
    size_t cacheLevel, const CacheAccess &access,
    const CacheAccessResult &prevLevelAccessResult,
    SimulationResult &simulationResult) {
  if (prevLevelAccessResult.ShouldWriteToSameAddressNextLevel()) {
    assert(!access.IsRead());
    this->ProcessAccessOnCacheLevelsRecursively(cacheLevel, access,
                                                simulationResult);
  }
}

void HierarchicalCacheState::HandleWriteBackOnCacheLevel(
    size_t cacheLevel, const CacheAccessResult &prevLevelAccessResult,
    SimulationResult &simulationResult) {
  if (prevLevelAccessResult.ShouldWriteToEvictedAddressNextLevel())
    this->ProcessAccessOnCacheLevelsRecursively(
        cacheLevel,
        prevLevelAccessResult.CreateCacheWriteAccessToEvictedAddress(),
        simulationResult);
}

size_t HierarchicalCacheState::GetNumberOfLines() const { return this->nLines; }

size_t HierarchicalCacheState::GetNumberOfNonEmptyLines() const {
  size_t n = 0;
  for (const auto &cs : this->cacheStates)
    n += cs->GetNumberOfNonEmptyLines();
  return n;
}

void HierarchicalCacheState::AddLinesToCacheStateSnapshot(
    std::vector<CacheStateSnapshot> &cssByCacheLevel) const {
  for (const auto &cs : this->cacheStates)
    cs->AddLinesToCacheStateSnapshot(cssByCacheLevel);
}

void HierarchicalCacheState::Warp(const WarpInfo &warpInfo) {
  for (const auto &cs : this->cacheStates)
    cs->Warp(warpInfo);
}

std::vector<std::unique_ptr<CacheState>>
HierarchicalCacheState::CreateCacheStates(
    const std::vector<CacheSettings> &cacheSettingsVec,
    const GlobalIteratorState &globalIteratorState) const {
  assert(cacheSettingsVec.size() > 1);
  std::vector<std::unique_ptr<CacheState>> res;
  res.reserve(cacheSettingsVec.size());
  for (const auto &cacheSettings : cacheSettingsVec)
    res.push_back(CacheStateFactory::CreateCacheState(cacheSettings,
                                                      globalIteratorState));
  return res;
}

size_t HierarchicalCacheState::ExtractNumberOfLines() const {
  size_t n = 0;
  for (const auto &cs : this->cacheStates)
    n += cs->GetNumberOfLines();
  return n;
}
