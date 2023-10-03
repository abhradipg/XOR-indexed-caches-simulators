#pragma once

#include <deque>

#include "CacheAccess.hpp"
#include "CacheState/CacheLine.hpp"
#include "CacheState/ReplacementPolicy.hpp"

class FIFOReplacementPolicy : public ReplacementPolicy {
public:
  FIFOReplacementPolicy(size_t nLines,
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
  std::deque<CacheLine> cacheLines;

  std::unordered_map<size_t, decltype(cacheLines)::iterator> cacheLineIterators;

  [[nodiscard]] CacheLine Evict();
};
