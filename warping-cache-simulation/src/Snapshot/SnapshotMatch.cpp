#include "SnapshotMatch.hpp"

SnapshotMatch::SnapshotMatch(std::shared_ptr<Snapshot> older,
                             std::shared_ptr<Snapshot> newer)
    : older(std::move(older)), newer(std::move(newer)),
      itValDiff(this->newer->GetIteratorValue() -
                this->older->GetIteratorValue()),
      nIterationsBetween(this->ComputeNIterationsBetween()),
      nRightRotationsBetweenByCacheLevel(
          this->ComputeNRightRotationsBetweenByCacheLevel()),
      hitMissCountDiff(this->ComputeHitMissCountDiff()) {
  assert(this->older->GetIteratorId() == this->newer->GetIteratorId());
  assert(this->older->GetIteratorStride() == this->newer->GetIteratorStride());
  assert(this->older->GetIteratorValue() != this->newer->GetIteratorValue());
}

long SnapshotMatch::GetItValDiff() const { return this->itValDiff; }

size_t SnapshotMatch::GetNIterationsBetween() const {
  return this->nIterationsBetween;
}

const std::valarray<size_t> &
SnapshotMatch::GetNRightRotationsBetweenByCacheLevel() const {
  return this->nRightRotationsBetweenByCacheLevel;
}

const HitMissCountByCacheLevel &SnapshotMatch::GetHitMissCountDiff() const {
  return this->hitMissCountDiff;
}

const std::shared_ptr<Snapshot> &SnapshotMatch::GetOlderSnapshot() const {
  return this->older;
}

const std::shared_ptr<Snapshot> &SnapshotMatch::GetNewerSnapshot() const {
  return this->newer;
}

size_t SnapshotMatch::GetIteratorId() const {
  assert(this->older->GetIteratorId() == this->newer->GetIteratorId());
  return this->older->GetIteratorId();
}

long SnapshotMatch::GetOlderItVal() const {
  return this->older->GetIteratorValue();
}

long SnapshotMatch::GetNewerItVal() const {
  return this->newer->GetIteratorValue();
}

bool SnapshotMatch::IsItIncreasing() const {
  return this->GetOlderItVal() < this->GetNewerItVal();
}

const std::vector<CacheLine> &
SnapshotMatch::GetOldLines(size_t cacheLevel) const {
  return this->older->GetCacheStateSnapshot(cacheLevel).GetLines();
}

const std::vector<CacheLine> &
SnapshotMatch::GetNewLines(size_t cacheLevel) const {
  return this->newer->GetCacheStateSnapshot(cacheLevel).GetLines();
}

void SnapshotMatch::WarpBothSnapshots(size_t nWarp) const {
  this->WarpBothSnapshotsStatistics(nWarp);
  this->WarpBothSnapshotsCacheLines(nWarp);
  this->RotateBothSnapshots(nWarp);
}

void SnapshotMatch::WarpOnlyNewerSnapshot(size_t nWarp) const {
  this->WarpOnlyNewerSnapshotStatistics(nWarp);
  this->WarpOnlyNewerSnapshotCacheLines(nWarp);
  this->RotateOnlyNewerSnapshot(nWarp);
}

void SnapshotMatch::WarpBothSnapshotsStatistics(size_t nWarp) const {
  this->older->WarpStatistics(this->itValDiff * (long)nWarp,
                              this->hitMissCountDiff * nWarp);
  this->newer->WarpStatistics(this->itValDiff * (long)nWarp,
                              this->hitMissCountDiff * nWarp);
}

void SnapshotMatch::WarpOnlyNewerSnapshotStatistics(size_t nWarp) const {
  this->newer->WarpStatistics(this->itValDiff * (long)nWarp,
                              this->hitMissCountDiff * nWarp);
}

void SnapshotMatch::WarpBothSnapshotsCacheLines(size_t nWarp) const {
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    this->WarpBothSnapshotsCacheLines(nWarp, i);
}

void SnapshotMatch::WarpBothSnapshotsCacheLines(size_t nWarp,
                                                size_t cacheLevel) const {
  const auto nLines = this->older->GetNumberOfLines(cacheLevel);
  assert(nLines == this->newer->GetNumberOfLines(cacheLevel));
  for (size_t i = 0; i < nLines; i++) {
    const auto oldCa = (long)this->older->GetConcreteAddress(i, cacheLevel);
    const auto newCa = (long)this->newer->GetConcreteAddress(i, cacheLevel);
    const long diff = (newCa - oldCa) * (long)nWarp;
    this->older->WarpCacheStateSnapshotLine(i, diff, cacheLevel);
    this->newer->WarpCacheStateSnapshotLine(i, diff, cacheLevel);
  }
}

void SnapshotMatch::WarpOnlyNewerSnapshotCacheLines(size_t nWarp) const {
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    this->WarpOnlyNewerSnapshotCacheLines(nWarp, i);
}

void SnapshotMatch::WarpOnlyNewerSnapshotCacheLines(size_t nWarp,
                                                    size_t cacheLevel) const {
  const auto nLines = this->newer->GetNumberOfLines(cacheLevel);
  for (size_t i = 0; i < nLines; i++) {
    const auto oldCa = (long)this->older->GetConcreteAddress(i, cacheLevel);
    const auto newCa = (long)this->newer->GetConcreteAddress(i, cacheLevel);
    const long diff = (newCa - oldCa) * (long)nWarp;
    this->newer->WarpCacheStateSnapshotLine(i, diff, cacheLevel);
  }
}

void SnapshotMatch::RotateBothSnapshots(size_t nWarp) const {
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    if (GlobalVars::nCacheSetsByCacheLevel[i] > 1)
      this->RotateBothSnapshots(nWarp, i);
}

void SnapshotMatch::RotateBothSnapshots(size_t nWarp, size_t cacheLevel) const {

  const auto nRotations =
      nWarp * this->nRightRotationsBetweenByCacheLevel[cacheLevel];
  this->older->RotateCacheStateSnapshot(nRotations, cacheLevel);
  this->newer->RotateCacheStateSnapshot(nRotations, cacheLevel);
}

void SnapshotMatch::RotateOnlyNewerSnapshot(size_t nWarp) const {
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    if (GlobalVars::nCacheSetsByCacheLevel[i] > 1)
      this->RotateOnlyNewerSnapshot(nWarp, i);
}

void SnapshotMatch::RotateOnlyNewerSnapshot(size_t nWarp,
                                            size_t cacheLevel) const {
  const auto nRotations =
      nWarp * this->nRightRotationsBetweenByCacheLevel[cacheLevel];
  this->newer->RotateCacheStateSnapshot(nRotations, cacheLevel);
}

size_t SnapshotMatch::ComputeNIterationsBetween() const {
  const auto stride = this->older->GetIteratorStride();
  const auto nIterations = this->itValDiff / stride;
  assert(nIterations > 0);
  return nIterations;
}

size_t SnapshotMatch::ComputeNRightRotationsBetween(size_t cacheLevel) const {
  assert(this->older != nullptr && this->newer != nullptr);
  const auto olderIndex = this->older->GetLastAccessedSetIndex(cacheLevel);
  const auto newerIndex = this->newer->GetLastAccessedSetIndex(cacheLevel);
  if (newerIndex >= olderIndex)
    return newerIndex - olderIndex;
  else {
    const auto nLeftRotations = olderIndex - newerIndex;
    return GlobalVars::nCacheSetsByCacheLevel[cacheLevel] - nLeftRotations;
  }
}

std::valarray<size_t>
SnapshotMatch::ComputeNRightRotationsBetweenByCacheLevel() const {
  std::valarray<size_t> res;
  res.resize(GlobalVars::nCacheLevels);
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    res[i] = this->ComputeNRightRotationsBetween(i);
  return res;
}

HitMissCountByCacheLevel SnapshotMatch::ComputeHitMissCountDiff() const {
  return this->newer->GetHitMissCount() - this->older->GetHitMissCount();
}
