#include "SetAssociativeCacheState.hpp"

SetAssociativeCacheState::SetAssociativeCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState)
    : CacheState(cacheSettings.cacheLevel), nLines(cacheSettings.nCacheLines),
      nSets(cacheSettings.nCacheSets),
      cacheSetStates(
          this->CreateCacheSetStates(cacheSettings, globalIteratorState)) {
  assert(GlobalVars::lineSize == cacheSettings.lineSize);
  assert(GlobalVars::nCacheSetsByCacheLevel[this->cacheLevel] ==
         cacheSettings.nCacheSets);
}

CacheAccessResult
SetAssociativeCacheState::ProcessAccess(const CacheAccess &access,
                                        SimulationResult &simulationResult) {
  auto setIndex = this->ComputeSetIndex(access);
  assert(this->cacheSetStates[setIndex] != nullptr);
  this->lastAccessedSetIndex = setIndex;
  return this->cacheSetStates[setIndex]->ProcessAccess(access,
                                                       simulationResult);
}

size_t SetAssociativeCacheState::GetNumberOfLines() const {
  return this->nLines;
}

size_t SetAssociativeCacheState::GetNumberOfNonEmptyLines() const {
  size_t n = 0;
  for (const auto &cacheSetState : this->cacheSetStates)
    n += cacheSetState->GetNumberOfNonEmptyLines();
  return n;
}

void SetAssociativeCacheState::AddLinesToCacheStateSnapshot(
    std::vector<CacheStateSnapshot> &cssByCacheLevel) const {
  assert(this->cacheLevel < cssByCacheLevel.size());
  cssByCacheLevel[this->cacheLevel].SetLastAccessedSetIndex(
      this->lastAccessedSetIndex);

  for (size_t i = this->lastAccessedSetIndex; i < this->nSets; i++)
    this->cacheSetStates[i]->AddLinesToCacheStateSnapshot(cssByCacheLevel);

  for (size_t i = 0; i < this->lastAccessedSetIndex; i++)
    this->cacheSetStates[i]->AddLinesToCacheStateSnapshot(cssByCacheLevel);
}

void SetAssociativeCacheState::Warp(const WarpInfo &warpInfo) {
  this->WarpCacheSets(warpInfo);
  this->RotateCacheSets(warpInfo.GetNRightCacheSetRotations(this->cacheLevel));
}

void SetAssociativeCacheState::WarpCacheSets(const WarpInfo &warpInfo) {
  for (const auto &cacheSetState : this->cacheSetStates)
    cacheSetState->Warp(warpInfo);
}

void SetAssociativeCacheState::RotateCacheSets(size_t nRightRotations) {
  assert(this->nSets > 1);
  const auto nRight = nRightRotations % this->nSets;
  const auto nLeft = (this->nSets - nRight) % this->nSets;
  nRight < nLeft ? this->RotateCacheSetsRight(nRight)
                 : this->RotateCacheSetsLeft(nLeft);
}

void SetAssociativeCacheState::RotateCacheSetsRight(size_t nRotations) {
  std::rotate(this->cacheSetStates.rbegin(),
              this->cacheSetStates.rbegin() + nRotations,
              this->cacheSetStates.rend());
}

void SetAssociativeCacheState::RotateCacheSetsLeft(size_t nRotations) {
  std::rotate(this->cacheSetStates.begin(),
              this->cacheSetStates.begin() + nRotations,
              this->cacheSetStates.end());
}

size_t
SetAssociativeCacheState::ComputeSetIndex(const CacheAccess &access) const {
  return access.GetMemoryBlockId() % this->nSets;
}

std::vector<std::unique_ptr<FullyAssociativeCacheState>>
SetAssociativeCacheState::CreateCacheSetStates(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState) const {
  std::vector<std::unique_ptr<FullyAssociativeCacheState>> res;
  res.reserve(cacheSettings.nCacheSets);
  const auto &cacheSetSettings = this->CreateCacheSetSettings(cacheSettings);
  assert(cacheSetSettings.IsFullyAssociative());
  for (size_t i = 0; i < cacheSettings.nCacheSets; i++)
    res.push_back(CacheStateFactory::CreateFullyAssociativeCacheState(
        cacheSetSettings, globalIteratorState));
  return res;
}

CacheSettings SetAssociativeCacheState::CreateCacheSetSettings(
    const CacheSettings &cacheSettings) const {
  CacheSettings cacheSetSettings = cacheSettings;
  cacheSetSettings.nCacheSets = 1;
  cacheSetSettings.nCacheLines = cacheSetSettings.associativity;
  cacheSetSettings.cacheSize =
      cacheSetSettings.associativity * cacheSetSettings.lineSize;
  assert(cacheSetSettings.IsValid());
  return cacheSetSettings;
}
