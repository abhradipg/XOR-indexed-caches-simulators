#include "FIFOReplacementPolicy.hpp"

FIFOReplacementPolicy::FIFOReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState)
    : ReplacementPolicy(nLines, globalIteratorState) {}

size_t FIFOReplacementPolicy::GetNumberOfNonEmptyLines() const {
  assert(this->cacheLines.size() == this->cacheLineIterators.size());
  return this->cacheLines.size();
}

void FIFOReplacementPolicy::AddLinesToCacheStateSnapshot(
    CacheStateSnapshot &cacheStateSnapshot) const {
  for (const auto &cl : this->cacheLines)
    cacheStateSnapshot.AddLine(cl, this->globalIteratorState);
}

void FIFOReplacementPolicy::Warp(const WarpInfo &warpInfo) {
  this->cacheLineIterators.clear();
  for (auto it = this->cacheLines.begin(); it != this->cacheLines.end(); it++) {
    it->Warp(warpInfo);
    assert(this->cacheLineIterators.count(it->GetMemoryBlockId()) == 0);
    this->cacheLineIterators.emplace(it->GetMemoryBlockId(), it);
  }
}

bool FIFOReplacementPolicy::DoesContainMemoryBlock(size_t memoryBlock) const {
  return this->cacheLineIterators.count(memoryBlock) == 1;
}

std::optional<CacheLine>
FIFOReplacementPolicy::AddNewCacheLineAndGetEvicted(const Address &address) {
  auto it = this->cacheLineIterators.find(address.GetMemoryBlockId());
  assert(it == this->cacheLineIterators.end());
  this->cacheLines.emplace_front(address);
  this->cacheLineIterators.emplace(address.GetMemoryBlockId(),
                                   this->cacheLines.begin());
  if (this->cacheLineIterators.size() > this->nLines) {
    assert((this->cacheLineIterators.size() - this->nLines) == 1);
    return this->Evict();
  }
  return std::nullopt;
}

void FIFOReplacementPolicy::AccessToExistingCacheLine(const Address &address) {
  auto it = this->cacheLineIterators.find(address.GetMemoryBlockId());
  const auto &clIt = (it->second);
  assert(clIt->GetMemoryBlockId() == address.GetMemoryBlockId());
  clIt->Access(address);
}

void FIFOReplacementPolicy::AccessToExistingCacheLineAndMarkDirty(
    const Address &address) {
  auto it = this->cacheLineIterators.find(address.GetMemoryBlockId());
  const auto &clIt = (it->second);
  assert(clIt->GetMemoryBlockId() == address.GetMemoryBlockId());
  clIt->AccessAndMarkDirty(address);
}

CacheLine FIFOReplacementPolicy::Evict() {
  assert(!this->cacheLines.empty());
  const auto it = std::prev(this->cacheLines.end());
  const auto &lruCl = *it;
  const auto mb = lruCl.GetMemoryBlockId();
  assert(this->DoesContainMemoryBlock(mb));
  const CacheLine evicted = this->cacheLines.back();
  this->cacheLineIterators.erase(mb);
  this->cacheLines.pop_back();
  return evicted;
}
