#pragma once

#include "CacheState/CacheLine.hpp"
#include "CacheState/ReplacementPolicy.hpp"
#include "Util/LRUContainer.hpp"

class LRUReplacementPolicy : public ReplacementPolicy {
public:
  LRUReplacementPolicy(size_t nLines,
                       const GlobalIteratorState &globalIteratorState);

  [[nodiscard]] size_t GetNumberOfNonEmptyLines() const override;

  [[nodiscard]] bool DoesContainMemoryBlock(size_t memoryBlock) const override;

  std::optional<CacheLine>
  AddNewCacheLineAndGetEvicted(const Address &address) override;

  void AccessToExistingCacheLine(const Address &address) override;

  void AccessToExistingCacheLineAndMarkDirty(const Address &address) override;

  void AddLinesToCacheStateSnapshot(
      CacheStateSnapshot &cacheStateSnapshot) const override;

  void Warp(const WarpInfo &warpInfo) override;

private:
  LRUContainer<CacheLine> cacheLines;

  [[nodiscard]] CacheLine Evict();
};
