#pragma once

#include <memory>
#include <vector>

#include "CacheState/CacheLine.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"
#include "Util/Util.hpp"

#define LEFT 0
#define RIGHT 1

class PLRUIndexTree {
public:
  PLRUIndexTree(size_t nLines, const GlobalIteratorState &globalIteratorState);

  void SwitchBitsOnPath(size_t leafIndex);

  void MarkLeafNodeAndItsAncestorsNonEmpty(size_t leafIndex);

  [[nodiscard]] size_t FindNewCacheLineIndexAndSwitchBitsOnPath();

  void AddLinesToCacheStateSnapshot(
      const std::vector<std::optional<CacheLine>> &leaves,
      CacheStateSnapshot &cacheStateSnapshot) const;

private:
  const GlobalIteratorState &globalIteratorState;

  std::vector<bool> bits;
  std::vector<bool> isBranchNonEmpty;
  std::vector<std::pair<size_t, size_t>> bitsLeftLeafIndexIntervals;
  std::vector<std::pair<size_t, size_t>> bitsRightLeafIndexIntervals;

  void
  InitBitsLeafIndexIntervalsRecursively(size_t bitIndex,
                                        std::pair<size_t, size_t> interval);

  [[nodiscard]] bool IsLeafIndex(size_t bitOrLeafIndex) const;

  [[nodiscard]] bool IsLeafIndexInLeft(size_t bitIndex, size_t leafIndex) const;

  [[nodiscard]] bool IsLeafIndexInRight(size_t bitIndex,
                                        size_t leafIndex) const;

  [[nodiscard]] size_t GetLeftIndex(size_t bitIndex) const;

  [[nodiscard]] size_t GetRightIndex(size_t bitIndex) const;

  [[nodiscard]] size_t GetBitNodeParentIndex(size_t bitIndex) const;

  [[nodiscard]] size_t GetLeafNodeParentIndex(size_t leafIndex) const;

  [[nodiscard]] size_t
  FindNewCacheLineIndexAndSwitchBitsOnPathRecursively(size_t bitIndex);

  void SwitchBitsOnPathRecursively(size_t bitIndex, size_t leafIndex);

  void MarkBranchAndParentBranchesNonEmpty(size_t bitIndex);

  void AddLinesToCacheStateSnapshotRecursively(
      size_t bitIndex, const std::vector<std::optional<CacheLine>> &leaves,
      CacheStateSnapshot &cacheStateSnapshot) const;

  void AddLineToCacheStateSnapshot(
      size_t leafIndex, const std::vector<std::optional<CacheLine>> &leaves,
      CacheStateSnapshot &cacheStateSnapshot) const;
};
