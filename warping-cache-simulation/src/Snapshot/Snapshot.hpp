#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "CacheState/CacheState.hpp"
#include "Simulation/SimulationResult.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"

class Snapshot {
public:
  Snapshot(const IteratorState &iteratorState,
           const SimulationResult &simulationResult,
           const CacheState &cacheState);

  [[nodiscard]] long GetIteratorValue() const;

  [[nodiscard]] long GetIteratorStride() const;

  [[nodiscard]] size_t GetIteratorId() const;

  [[nodiscard]] const HitMissCountByCacheLevel &GetHitMissCount() const;

  [[nodiscard]] size_t GetNumberOfLines(size_t cacheLevel) const;

  [[nodiscard]] size_t GetConcreteAddress(size_t lineIndex,
                                          size_t cacheLevel) const;

  [[nodiscard]] size_t GetSymbolicHash() const;

  [[nodiscard]] size_t GetLastAccessedSetIndex(size_t cacheLevel) const;

  [[nodiscard]] bool DoesHaveSingleCoefficient() const;

  [[nodiscard]] long GetSingleCoefficient() const;

  [[nodiscard]] const CacheStateSnapshot &
  GetCacheStateSnapshot(size_t cacheLevel) const;

  [[nodiscard]] bool IsSymbolicallyEqual(const Snapshot &other) const;

  void WarpStatistics(long itValDiff,
                      const HitMissCountByCacheLevel &hitMissCountDiff);

  void WarpCacheStateSnapshotLine(size_t lineIndex, long concreteAddressDiff,
                                  size_t cacheLevel);

  void RotateCacheStateSnapshot(size_t nRightRotations, size_t cacheLevel);

  void InitCacheStateSnapshotByCacheLevel(const CacheState &cacheState);

  void InitSymbolicHash();

private:
  size_t iteratorId;
  long iteratorValue;
  long iteratorStride;
  HitMissCountByCacheLevel hitMissCount;
  std::vector<CacheStateSnapshot> cssByCacheLevel;
  size_t symbolicHash;
};
