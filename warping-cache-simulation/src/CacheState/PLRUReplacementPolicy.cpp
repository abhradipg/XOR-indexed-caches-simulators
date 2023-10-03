#include "PLRUReplacementPolicy.hpp"

PLRUReplacementPolicy::PLRUReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState)
    : ReplacementPolicy(nLines, globalIteratorState),
      plruIndexTree(nLines, globalIteratorState) {
  this->leaves.resize(nLines);
}

size_t PLRUReplacementPolicy::GetNumberOfNonEmptyLines() const {
  return this->nonEmptyLeafIndices.size();
}

void PLRUReplacementPolicy::AddLinesToCacheStateSnapshot(
    CacheStateSnapshot &cacheStateSnapshot) const {
  this->plruIndexTree.AddLinesToCacheStateSnapshot(this->leaves,
                                                   cacheStateSnapshot);
}

void PLRUReplacementPolicy::Warp(const WarpInfo &warpInfo) {
  std::unordered_map<size_t, size_t> newLeafIndices;
  for (auto &el : this->nonEmptyLeafIndices) {
    const auto leafIndex = el.second;
    auto &cl = this->leaves[leafIndex];
    assert(cl && cl->GetMemoryBlockId() == el.first);
    cl->Warp(warpInfo);
    assert(newLeafIndices.count(cl->GetMemoryBlockId()) == 0);
    newLeafIndices.emplace(cl->GetMemoryBlockId(), el.second);
  }
  this->nonEmptyLeafIndices = std::move(newLeafIndices);
}

bool PLRUReplacementPolicy::DoesContainMemoryBlock(size_t memoryBlock) const {
  return this->nonEmptyLeafIndices.count(memoryBlock) == 1;
}

std::optional<CacheLine>
PLRUReplacementPolicy::AddNewCacheLineAndGetEvicted(const Address &address) {
  const auto leafIndex =
      this->plruIndexTree.FindNewCacheLineIndexAndSwitchBitsOnPath();
  auto &cl = this->leaves[leafIndex];

  std::optional<CacheLine> evicted;
  const auto isLineEmpty = !cl;
  if (isLineEmpty) {
    cl.emplace(address);
    this->plruIndexTree.MarkLeafNodeAndItsAncestorsNonEmpty(leafIndex);
  } else {
    evicted.emplace(cl.value());
    this->Evict(cl->GetMemoryBlockId());
    cl->Access(address);
  }

  this->nonEmptyLeafIndices[address.GetMemoryBlockId()] = leafIndex;
  return evicted;
}

void PLRUReplacementPolicy::AccessToExistingCacheLine(const Address &address) {
  const auto leafIndex = this->nonEmptyLeafIndices[address.GetMemoryBlockId()];
  this->plruIndexTree.SwitchBitsOnPath(leafIndex);
  this->leaves[leafIndex]->Access(address);
}

void PLRUReplacementPolicy::AccessToExistingCacheLineAndMarkDirty(
    const Address &address) {
  const auto leafIndex = this->nonEmptyLeafIndices[address.GetMemoryBlockId()];
  this->plruIndexTree.SwitchBitsOnPath(leafIndex);
  this->leaves[leafIndex]->AccessAndMarkDirty(address);
}

void PLRUReplacementPolicy::Evict(size_t memoryBlockId) {
  this->nonEmptyLeafIndices.erase(memoryBlockId);
}
