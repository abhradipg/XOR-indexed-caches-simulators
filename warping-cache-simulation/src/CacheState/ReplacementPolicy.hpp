#pragma once

#include <cstddef>

#include "Address/Address.hpp"
#include "IteratorState/GlobalIteratorState.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"
#include "Warping/WarpInfo.hpp"

class ReplacementPolicy {
public:
  ReplacementPolicy(size_t nLines,
                    const GlobalIteratorState &globalIteratorState);

  virtual ~ReplacementPolicy() = default;

  [[nodiscard]] bool IsFull() const;

  [[nodiscard]] virtual size_t GetNumberOfNonEmptyLines() const = 0;

  [[nodiscard]] virtual bool
  DoesContainMemoryBlock(size_t memoryBlock) const = 0;

  [[nodiscard]] virtual std::optional<CacheLine>
  AddNewCacheLineAndGetEvicted(const Address &address) = 0;

  virtual void AccessToExistingCacheLine(const Address &address) = 0;

  virtual void
  AccessToExistingCacheLineAndMarkDirty(const Address &address) = 0;

  virtual void AddLinesToCacheStateSnapshot(
      CacheStateSnapshot &cacheStateSnapshot) const = 0;

  virtual void Warp(const WarpInfo &warpInfo) = 0;

protected:
  const size_t nLines;

  const GlobalIteratorState &globalIteratorState;
};
