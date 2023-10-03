#include "HitMissCountByCacheLevel.hpp"

HitMissCountByCacheLevel::HitMissCountByCacheLevel()
    : HitMissCountByCacheLevel(1) {}

HitMissCountByCacheLevel::HitMissCountByCacheLevel(size_t nCacheLevels) {
  assert(nCacheLevels > 0);
  this->cacheLevelToHitCount.resize(nCacheLevels, 0);
  this->cacheLevelToMissCount.resize(nCacheLevels, 0);
}

HitMissCountByCacheLevel::HitMissCountByCacheLevel(
    std::valarray<size_t> cacheLevelToHitCount,
    std::valarray<size_t> cacheLevelToMissCount)
    : cacheLevelToHitCount(std::move(cacheLevelToHitCount)),
      cacheLevelToMissCount(std::move(cacheLevelToMissCount)) {}

size_t HitMissCountByCacheLevel::GetHitCount(size_t cacheLevel) const {
  assert(cacheLevel < this->cacheLevelToHitCount.size());
  return this->cacheLevelToHitCount[cacheLevel];
}

size_t HitMissCountByCacheLevel::GetMissCount(size_t cacheLevel) const {
  assert(cacheLevel < this->cacheLevelToMissCount.size());
  return this->cacheLevelToMissCount[cacheLevel];
}

size_t HitMissCountByCacheLevel::GetNCacheLevels() const {
  return this->cacheLevelToHitCount.size();
}

size_t HitMissCountByCacheLevel::GetAccessCount(size_t cacheLevel) const {
  assert(cacheLevel < this->GetNCacheLevels());
  return this->GetHitCount(cacheLevel) + this->GetMissCount(cacheLevel);
}

const std::valarray<size_t> &HitMissCountByCacheLevel::GetHitCounts() const {
  return this->cacheLevelToHitCount;
}

const std::valarray<size_t> &HitMissCountByCacheLevel::GetMissCounts() const {
  return this->cacheLevelToMissCount;
}

void HitMissCountByCacheLevel::AddHit(size_t cacheLevel) {
  assert(cacheLevel < this->cacheLevelToHitCount.size());
  this->cacheLevelToHitCount[cacheLevel]++;
}

void HitMissCountByCacheLevel::AddMiss(size_t cacheLevel) {
  assert(cacheLevel < this->cacheLevelToMissCount.size());
  this->cacheLevelToMissCount[cacheLevel]++;
}

bool HitMissCountByCacheLevel::operator==(
    const HitMissCountByCacheLevel &other) const {
  return (this->cacheLevelToHitCount == other.cacheLevelToHitCount).min() &&
         (this->cacheLevelToMissCount == other.cacheLevelToMissCount).min();
}

HitMissCountByCacheLevel
HitMissCountByCacheLevel::operator*(size_t times) const {
  return HitMissCountByCacheLevel(this->cacheLevelToHitCount * times,
                                  this->cacheLevelToMissCount * times);
}

HitMissCountByCacheLevel &HitMissCountByCacheLevel::operator*=(size_t times) {
  this->cacheLevelToHitCount *= times;
  this->cacheLevelToMissCount *= times;
  return *this;
}

HitMissCountByCacheLevel HitMissCountByCacheLevel::operator+(
    const HitMissCountByCacheLevel &other) const {
  assert(this->GetNCacheLevels() == other.GetNCacheLevels());
  return HitMissCountByCacheLevel(
      this->cacheLevelToHitCount + other.cacheLevelToHitCount,
      this->cacheLevelToMissCount + other.cacheLevelToMissCount);
}

HitMissCountByCacheLevel &
HitMissCountByCacheLevel::operator+=(const HitMissCountByCacheLevel &other) {
  assert(this->GetNCacheLevels() == other.GetNCacheLevels());
  this->cacheLevelToHitCount += other.cacheLevelToHitCount;
  this->cacheLevelToMissCount += other.cacheLevelToMissCount;
  return *this;
}

HitMissCountByCacheLevel HitMissCountByCacheLevel::operator-(
    const HitMissCountByCacheLevel &other) const {
  assert(this->GetNCacheLevels() == other.GetNCacheLevels());
  return HitMissCountByCacheLevel(
      this->cacheLevelToHitCount - other.cacheLevelToHitCount,
      this->cacheLevelToMissCount - other.cacheLevelToMissCount);
}
