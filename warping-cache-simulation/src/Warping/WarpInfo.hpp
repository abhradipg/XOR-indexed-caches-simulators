#pragma once

#include <cstddef>
#include <valarray>

#include "CacheState/HitMissCountByCacheLevel.hpp"

class WarpInfo {
public:
  WarpInfo(size_t itId, size_t nItAdvance, long itValDiff,
           HitMissCountByCacheLevel hitMissCount,
           std::valarray<size_t> nRightRotationsByCacheLevel);

  [[nodiscard]] size_t GetItId() const;

  [[nodiscard]] size_t GetNItAdvance() const;

  [[nodiscard]] size_t GetNRightCacheSetRotations(size_t cacheLevel) const;

  [[nodiscard]] long GetItValDiff() const;

  [[nodiscard]] const HitMissCountByCacheLevel &GetHitMissCount() const;

private:
  const size_t itId;
  const size_t nItAdvance;
  const long itValDiff;
  const HitMissCountByCacheLevel hitMissCount;
  const std::valarray<size_t> nRightRotationsByCacheLevel;
};
