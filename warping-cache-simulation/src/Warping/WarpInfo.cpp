#include "WarpInfo.hpp"

WarpInfo::WarpInfo(size_t itId, size_t nItAdvance, long itValDiff,
                   HitMissCountByCacheLevel hitMissCount,
                   std::valarray<size_t> nRightRotationsByCacheLevel)
    : itId(itId), nItAdvance(nItAdvance), itValDiff(itValDiff),
      hitMissCount(std::move(hitMissCount)),
      nRightRotationsByCacheLevel(std::move(nRightRotationsByCacheLevel))

{}
size_t WarpInfo::GetItId() const { return itId; }

const HitMissCountByCacheLevel &WarpInfo::GetHitMissCount() const {
  return this->hitMissCount;
}

size_t WarpInfo::GetNItAdvance() const { return nItAdvance; }

size_t WarpInfo::GetNRightCacheSetRotations(size_t cacheLevel) const {
  return nRightRotationsByCacheLevel[cacheLevel];
}

long WarpInfo::GetItValDiff() const { return itValDiff; }
