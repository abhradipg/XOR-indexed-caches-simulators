#pragma once

#include <memory>
#include <unordered_map>

#include "CacheState/CacheLine.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Util/GlobalVars.hpp"

class SnapshotMatch {
public:
  SnapshotMatch(std::shared_ptr<Snapshot> older,
                std::shared_ptr<Snapshot> newer);

  [[nodiscard]] long GetItValDiff() const;

  [[nodiscard]] size_t GetNIterationsBetween() const;

  [[nodiscard]] const std::valarray<size_t> &
  GetNRightRotationsBetweenByCacheLevel() const;

  [[nodiscard]] const HitMissCountByCacheLevel &GetHitMissCountDiff() const;

  [[nodiscard]] const std::shared_ptr<Snapshot> &GetOlderSnapshot() const;

  [[nodiscard]] const std::shared_ptr<Snapshot> &GetNewerSnapshot() const;

  [[nodiscard]] size_t GetIteratorId() const;

  [[nodiscard]] long GetOlderItVal() const;

  [[nodiscard]] long GetNewerItVal() const;

  [[nodiscard]] bool IsItIncreasing() const;

  [[nodiscard]] const std::vector<CacheLine> &
  GetOldLines(size_t cacheLevel) const;

  [[nodiscard]] const std::vector<CacheLine> &
  GetNewLines(size_t cacheLevel) const;

  void WarpBothSnapshots(size_t nWarp) const;

  void WarpOnlyNewerSnapshot(size_t nWarp) const;

private:
  std::shared_ptr<Snapshot> older;
  std::shared_ptr<Snapshot> newer;
  const long itValDiff;
  const size_t nIterationsBetween;
  const std::valarray<size_t> nRightRotationsBetweenByCacheLevel;
  const HitMissCountByCacheLevel hitMissCountDiff;

  void WarpBothSnapshotsStatistics(size_t nWarp) const;

  void WarpOnlyNewerSnapshotStatistics(size_t nWarp) const;

  void WarpBothSnapshotsCacheLines(size_t nWarp) const;

  void WarpBothSnapshotsCacheLines(size_t nWarp, size_t cacheLevel) const;

  void WarpOnlyNewerSnapshotCacheLines(size_t nWarp) const;

  void WarpOnlyNewerSnapshotCacheLines(size_t nWarp, size_t cacheLevel) const;

  void RotateBothSnapshots(size_t nWarp) const;

  void RotateBothSnapshots(size_t nWarp, size_t cacheLevel) const;

  void RotateOnlyNewerSnapshot(size_t nWarp) const;

  void RotateOnlyNewerSnapshot(size_t nWarp, size_t cacheLevel) const;

  [[nodiscard]] size_t ComputeNIterationsBetween() const;

  [[nodiscard]] size_t ComputeNRightRotationsBetween(size_t cacheLevel) const;

  [[nodiscard]] std::valarray<size_t>
  ComputeNRightRotationsBetweenByCacheLevel() const;

  [[nodiscard]] HitMissCountByCacheLevel ComputeHitMissCountDiff() const;
};
