#include "LRU3PLRU4ReplacementPolicy.hpp"

LRU3PLRU4ReplacementPolicy::LRU3PLRU4ReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState)
    : ReplacementPolicy(nLines, globalIteratorState) {
  this->InitPlruReplacementPolicies(nLines, globalIteratorState);
}

size_t LRU3PLRU4ReplacementPolicy::GetNumberOfNonEmptyLines() const {
  size_t n = 0;
  for (const auto &el : this->plruReplacementPolicies.GetIdElPairsConstRef()) {
    const auto &plru = el.second;
    n += plru.GetNumberOfNonEmptyLines();
  }
  return n;
}

bool LRU3PLRU4ReplacementPolicy::DoesContainMemoryBlock(
    size_t memoryBlock) const {
  for (const auto &el : this->plruReplacementPolicies.GetIdElPairsConstRef()) {
    const auto &plru = el.second;
    if (plru.DoesContainMemoryBlock(memoryBlock))
      return true;
  }
  return false;
}

std::optional<CacheLine>
LRU3PLRU4ReplacementPolicy::AddNewCacheLineAndGetEvicted(
    const Address &address) {
  auto &plru = this->plruReplacementPolicies.UseLruAndGetUsedElRef();
  return plru.AddNewCacheLineAndGetEvicted(address);
}

void LRU3PLRU4ReplacementPolicy::AccessToExistingCacheLine(
    const Address &address) {
  const auto mb = address.GetMemoryBlockId();
  auto &plruWithId = this->GetPlruWithIdContainingMemoryBlock(mb);
  plruWithId.second.AccessToExistingCacheLine(address);
  this->plruReplacementPolicies.Use(plruWithId.first);
}

void LRU3PLRU4ReplacementPolicy::AccessToExistingCacheLineAndMarkDirty(
    const Address &address) {
  const auto mb = address.GetMemoryBlockId();
  auto &plruWithId = this->GetPlruWithIdContainingMemoryBlock(mb);
  plruWithId.second.AccessToExistingCacheLineAndMarkDirty(address);
  this->plruReplacementPolicies.Use(plruWithId.first);
}

void LRU3PLRU4ReplacementPolicy::AddLinesToCacheStateSnapshot(
    CacheStateSnapshot &cacheStateSnapshot) const {
  for (const auto &el : this->plruReplacementPolicies.GetIdElPairsConstRef()) {
    const auto &plru = el.second;
    plru.AddLinesToCacheStateSnapshot(cacheStateSnapshot);
  }
}

void LRU3PLRU4ReplacementPolicy::Warp(const WarpInfo &warpInfo) {
  for (auto &el : this->plruReplacementPolicies.GetIdElPairsRef()) {
    auto &plru = el.second;
    plru.Warp(warpInfo);
  }
}

void LRU3PLRU4ReplacementPolicy::InitPlruReplacementPolicies(
    size_t nLines, const GlobalIteratorState &globalIteratorState) {
  assert(nLines % 3 == 0);
  size_t plruNLines = nLines / 3;
  for (size_t i = 0; i < 3; i++)
    this->plruReplacementPolicies.Add(i, {plruNLines, globalIteratorState});
}

std::pair<size_t, PLRUReplacementPolicy> &
LRU3PLRU4ReplacementPolicy::GetPlruWithIdContainingMemoryBlock(size_t mb) {
  for (auto &el : this->plruReplacementPolicies.GetIdElPairsRef())
    if (el.second.DoesContainMemoryBlock(mb))
      return el;
  assert(false);
}
