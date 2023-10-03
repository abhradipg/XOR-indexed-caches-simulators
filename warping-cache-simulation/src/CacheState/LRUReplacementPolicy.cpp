#include "LRUReplacementPolicy.hpp"

LRUReplacementPolicy::LRUReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState)
    : ReplacementPolicy(nLines, globalIteratorState) {}

size_t LRUReplacementPolicy::GetNumberOfNonEmptyLines() const {
  return this->cacheLines.GetSize();
}

bool LRUReplacementPolicy::DoesContainMemoryBlock(size_t memoryBlock) const {
  return this->cacheLines.DoesContain(memoryBlock);
}

std::optional<CacheLine>
LRUReplacementPolicy::AddNewCacheLineAndGetEvicted(const Address &address) {
  this->cacheLines.Add(address.GetMemoryBlockId(), CacheLine(address));
  if (this->cacheLines.GetSize() > this->nLines)
    return this->Evict();
  return std::nullopt;
}

void LRUReplacementPolicy::AccessToExistingCacheLine(const Address &address) {
  const auto mb = address.GetMemoryBlockId();
  auto &clIt = this->cacheLines.UseAndGetUsedElRef(mb);
  clIt.Access(address);
  assert(&this->cacheLines.GetMruElConstRef() == &clIt);
}

void LRUReplacementPolicy::AccessToExistingCacheLineAndMarkDirty(
    const Address &address) {
  const auto mb = address.GetMemoryBlockId();
  auto &clIt = this->cacheLines.UseAndGetUsedElRef(mb);
  clIt.AccessAndMarkDirty(address);
  assert(&this->cacheLines.GetMruElConstRef() == &clIt);
}

void LRUReplacementPolicy::AddLinesToCacheStateSnapshot(
    CacheStateSnapshot &cacheStateSnapshot) const {
  for (const auto &el : this->cacheLines.GetIdElPairsConstRef())
    cacheStateSnapshot.AddLine(el.second, this->globalIteratorState);
}

void LRUReplacementPolicy::Warp(const WarpInfo &warpInfo) {
  auto &idLinePairMap = this->cacheLines.GetIdElPairItMapRef();
  auto &idLinePairs = this->cacheLines.GetIdElPairsRef();
  idLinePairMap.clear();
  for (auto it = idLinePairs.begin(); it != idLinePairs.end(); it++) {
    auto &mb = it->first;
    auto &line = it->second;
    assert(mb == line.GetMemoryBlockId());
    line.Warp(warpInfo);
    mb = line.GetMemoryBlockId();
    assert(idLinePairMap.count(mb) == 0);
    idLinePairMap.emplace(mb, it);
  }
}

CacheLine LRUReplacementPolicy::Evict() {
  const CacheLine evicted = this->cacheLines.GetLruElConstRef();
  this->cacheLines.RemoveLru();
  return evicted;
}
