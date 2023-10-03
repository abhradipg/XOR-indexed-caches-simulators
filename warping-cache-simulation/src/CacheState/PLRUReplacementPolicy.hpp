#pragma once

#include <cmath>
#include <list>
#include <optional>

#include "CacheState/CacheLine.hpp"
#include "CacheState/PLRUIndexTree.hpp"
#include "CacheState/ReplacementPolicy.hpp"

class PLRUReplacementPolicy : public ReplacementPolicy {
public:
  PLRUReplacementPolicy(size_t nLines,
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
  PLRUIndexTree plruIndexTree;
  std::vector<std::optional<CacheLine>> leaves;
  std::unordered_map<size_t, size_t> nonEmptyLeafIndices;

  void Evict(size_t memoryBlockId);
};
