#pragma once

#include "CacheState/CacheLine.hpp"
#include "CacheState/PLRUReplacementPolicy.hpp"
#include "CacheState/ReplacementPolicy.hpp"
#include "Util/LRUContainer.hpp"

class LRU3PLRU4ReplacementPolicy : public ReplacementPolicy {
public:
  LRU3PLRU4ReplacementPolicy(size_t nLines,
                             const GlobalIteratorState &globalIteratorState);

  [[nodiscard]] size_t GetNumberOfNonEmptyLines() const override;

  [[nodiscard]] bool DoesContainMemoryBlock(size_t memoryBlock) const override;

  [[nodiscard]] std::optional<CacheLine>
  AddNewCacheLineAndGetEvicted(const Address &address) override;

  void AccessToExistingCacheLine(const Address &address) override;

  void AccessToExistingCacheLineAndMarkDirty(const Address &address) override;

  void AddLinesToCacheStateSnapshot(
      CacheStateSnapshot &cacheStateSnapshot) const override;

  void Warp(const WarpInfo &warpInfo) override;

private:
  LRUContainer<PLRUReplacementPolicy> plruReplacementPolicies;

  void
  InitPlruReplacementPolicies(size_t nLines,
                              const GlobalIteratorState &globalIteratorState);

  std::pair<size_t, PLRUReplacementPolicy> &
  GetPlruWithIdContainingMemoryBlock(size_t mb);
};
