#include "FullyAssociativeCacheState.hpp"

FullyAssociativeCacheState::FullyAssociativeCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState)
    : CacheState(cacheSettings.cacheLevel), nLines(cacheSettings.nCacheLines),
      replacementPolicy(ReplacementPolicyFactory::CreateReplacementPolicy(
          cacheSettings, globalIteratorState)),
      writePolicy(cacheSettings.writePolicy) {
  assert(cacheSettings.nCacheSets == 1);
  assert(cacheSettings.associativity == cacheSettings.nCacheLines);
}

CacheAccessResult
FullyAssociativeCacheState::ProcessAccess(const CacheAccess &access,
                                          SimulationResult &simulationResult) {
  if (this->IsHit(access)) {
    simulationResult.AddSimulatedHit(this->cacheLevel);
    return this->ProcessHit(access);
  } else {
    simulationResult.AddSimulatedMiss(this->cacheLevel);
    return this->ProcessMiss(access);
  }
}

size_t FullyAssociativeCacheState::GetNumberOfLines() const {
  return this->nLines;
}

size_t FullyAssociativeCacheState::GetNumberOfNonEmptyLines() const {
  return this->replacementPolicy->GetNumberOfNonEmptyLines();
}

void FullyAssociativeCacheState::AddLinesToCacheStateSnapshot(
    std::vector<CacheStateSnapshot> &cssByCacheLevel) const {
  assert(this->cacheLevel < cssByCacheLevel.size());
  this->replacementPolicy->AddLinesToCacheStateSnapshot(
      cssByCacheLevel[cacheLevel]);
}

void FullyAssociativeCacheState::Warp(const WarpInfo &warpInfo) {
  this->replacementPolicy->Warp(warpInfo);
}

bool FullyAssociativeCacheState::IsHit(const CacheAccess &access) const {
  return this->replacementPolicy->DoesContainMemoryBlock(
      access.GetMemoryBlockId());
}

CacheAccessResult
FullyAssociativeCacheState::ProcessHit(const CacheAccess &access) {
  const auto &address = access.GetAddress();
  return access.IsRead() ? this->ProcessReadHit(address)
                         : this->ProcessWriteHit(address);
}

CacheAccessResult
FullyAssociativeCacheState::ProcessMiss(const CacheAccess &access) {
  const auto &address = access.GetAddress();
  return access.IsRead() ? this->ProcessReadMiss(address)
                         : this->ProcessWriteMiss(address);
}

CacheAccessResult
FullyAssociativeCacheState::ProcessReadHit(const Address &address) {
  this->replacementPolicy->AccessToExistingCacheLine(address);
  return CacheAccessResult::Hit(false);
}

CacheAccessResult
FullyAssociativeCacheState::ProcessReadMiss(const Address &address) {
  const auto &evicted =
      this->replacementPolicy->AddNewCacheLineAndGetEvicted(address);
  return evicted ? CacheAccessResult::CapacityMiss(evicted.value())
                 : CacheAccessResult::ColdMiss();
}

CacheAccessResult
FullyAssociativeCacheState::ProcessWriteHit(const Address &address) {
  if (this->IsWriteBack()) {
    this->replacementPolicy->AccessToExistingCacheLineAndMarkDirty(address);
    return CacheAccessResult::Hit(false);
  } else {
    assert(this->IsWriteThrough());
    this->replacementPolicy->AccessToExistingCacheLine(address);
    return CacheAccessResult::Hit(true);
  }
}

CacheAccessResult
FullyAssociativeCacheState::ProcessWriteMiss(const Address &address) {
  if (this->IsWriteAllocate()) {
    const auto &evicted =
        this->replacementPolicy->AddNewCacheLineAndGetEvicted(address);
    return evicted ? CacheAccessResult::CapacityMiss(evicted.value())
                   : CacheAccessResult::ColdMiss();
  } else {
    assert(this->IsNoWriteAllocate()); // data is not loaded to the cache
    return this->replacementPolicy->IsFull() ? CacheAccessResult::CapacityMiss()
                                             : CacheAccessResult::ColdMiss();
  }
}

bool FullyAssociativeCacheState::IsWriteBack() const {
  return this->writePolicy == CacheSettings::WRITE_BACK_WRITE_ALLOCATE;
}

bool FullyAssociativeCacheState::IsWriteThrough() const {
  return this->writePolicy == CacheSettings::WRITE_THROUGH_NO_WRITE_ALLOCATE;
}

bool FullyAssociativeCacheState::IsWriteAllocate() const {
  return this->writePolicy == CacheSettings::WRITE_BACK_WRITE_ALLOCATE;
}

bool FullyAssociativeCacheState::IsNoWriteAllocate() const {
  return this->writePolicy == CacheSettings::WRITE_THROUGH_NO_WRITE_ALLOCATE;
}
