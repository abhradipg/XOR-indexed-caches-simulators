#pragma once

#include <cassert>
#include <valarray>

class HitMissCountByCacheLevel {
public:
  HitMissCountByCacheLevel();

  explicit HitMissCountByCacheLevel(size_t nCacheLevels);

  [[nodiscard]] size_t GetNCacheLevels() const;

  [[nodiscard]] size_t GetHitCount(size_t cacheLevel) const;
  [[nodiscard]] size_t GetMissCount(size_t cacheLevel) const;
  [[nodiscard]] size_t GetAccessCount(size_t cacheLevel) const;

  [[nodiscard]] const std::valarray<size_t> &GetHitCounts() const;
  [[nodiscard]] const std::valarray<size_t> &GetMissCounts() const;

  void AddHit(size_t cacheLevel);
  void AddMiss(size_t cacheLevel);

  bool operator==(const HitMissCountByCacheLevel &other) const;

  HitMissCountByCacheLevel operator*(size_t times) const;

  HitMissCountByCacheLevel &operator*=(size_t times);

  HitMissCountByCacheLevel
  operator+(const HitMissCountByCacheLevel &other) const;

  HitMissCountByCacheLevel &operator+=(const HitMissCountByCacheLevel &other);

  HitMissCountByCacheLevel
  operator-(const HitMissCountByCacheLevel &other) const;

private:
  std::valarray<size_t> cacheLevelToHitCount;
  std::valarray<size_t> cacheLevelToMissCount;

  HitMissCountByCacheLevel(std::valarray<size_t> cacheLevelToHitCount,
                           std::valarray<size_t> cacheLevelToMissCount);
};
