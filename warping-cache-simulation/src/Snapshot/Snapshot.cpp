#include "Snapshot.hpp"

Snapshot::Snapshot(const IteratorState &iteratorState,
                   const SimulationResult &simulationResult,
                   const CacheState &cacheState)
    : iteratorId(iteratorState.GetIteratorId()),
      iteratorValue(iteratorState.GetCurrentValue()),
      iteratorStride(iteratorState.GetStride()),
      hitMissCount(simulationResult.GetHitMissCount()) {
  this->InitCacheStateSnapshotByCacheLevel(cacheState);
  this->InitSymbolicHash();
}

long Snapshot::GetIteratorValue() const { return this->iteratorValue; }

long Snapshot::GetIteratorStride() const { return this->iteratorStride; }

size_t Snapshot::GetIteratorId() const { return this->iteratorId; }

const HitMissCountByCacheLevel &Snapshot::GetHitMissCount() const {
  return this->hitMissCount;
}

size_t Snapshot::GetNumberOfLines(size_t cacheLevel) const {
  return this->cssByCacheLevel[cacheLevel].GetLines().size();
}

size_t Snapshot::GetConcreteAddress(size_t lineIndex, size_t cacheLevel) const {
  return this->cssByCacheLevel[cacheLevel].GetConcreteAddress(lineIndex);
}

size_t Snapshot::GetSymbolicHash() const { return symbolicHash; }

size_t Snapshot::GetLastAccessedSetIndex(size_t cacheLevel) const {
  return this->cssByCacheLevel[cacheLevel].GetLastAccessedSetIndex();
}

const CacheStateSnapshot &
Snapshot::GetCacheStateSnapshot(size_t cacheLevel) const {
  return this->cssByCacheLevel[cacheLevel];
}

bool Snapshot::IsSymbolicallyEqual(const Snapshot &other) const {
  assert(this->cssByCacheLevel.size() == other.cssByCacheLevel.size());
  for (size_t i = 0; i < this->cssByCacheLevel.size(); i++)
    if (!this->cssByCacheLevel[i].IsSymbolicallyEqual(other.cssByCacheLevel[i]))
      return false;
  return true;
}

bool Snapshot::DoesHaveSingleCoefficient() const {
  if (!this->cssByCacheLevel[0].DoesHaveSingleCoefficient())
    return false;

  const auto c = this->cssByCacheLevel[0].GetSingleCoefficient();
  for (const auto &css : this->cssByCacheLevel)
    if (!css.DoesHaveSingleCoefficient() || c != css.GetSingleCoefficient())
      return false;

  return true;
}

long Snapshot::GetSingleCoefficient() const {
  return this->cssByCacheLevel[0].GetSingleCoefficient();
}

void Snapshot::WarpStatistics(
    long itValDiff, const HitMissCountByCacheLevel &hitMissCountDiff) {
  this->iteratorValue += itValDiff;
  this->hitMissCount += hitMissCountDiff;
}

void Snapshot::WarpCacheStateSnapshotLine(size_t lineIndex,
                                          long concreteAddressDiff,
                                          size_t cacheLevel) {
  this->cssByCacheLevel[cacheLevel].WarpCacheLine(lineIndex,
                                                  concreteAddressDiff);
}

void Snapshot::RotateCacheStateSnapshot(size_t nRightRotations,
                                        size_t cacheLevel) {
  this->cssByCacheLevel[cacheLevel].Rotate(nRightRotations);
}

void Snapshot::InitCacheStateSnapshotByCacheLevel(
    const CacheState &cacheState) {
  this->cssByCacheLevel.reserve(GlobalVars::nCacheLevels);
  for (size_t i = 0; i < GlobalVars::nCacheLevels; i++)
    this->cssByCacheLevel.emplace_back(GlobalVars::nCacheLinesByCacheLevel[i],
                                       GlobalVars::nCacheSetsByCacheLevel[i],
                                       this->iteratorId);
  cacheState.AddLinesToCacheStateSnapshot(this->cssByCacheLevel);
}

void Snapshot::InitSymbolicHash() {
  this->symbolicHash = this->cssByCacheLevel.size();
  for (const auto &css : this->cssByCacheLevel)
    Util::HashCombine(css.GetSymbolicHash(), this->symbolicHash);
}
