#include "CacheAccessResult.hpp"

CacheAccessResult CacheAccessResult::Hit(bool writeToSameAddressNextLevel) {
  return CacheAccessResult(writeToSameAddressNextLevel);
}

CacheAccessResult CacheAccessResult::ColdMiss() {
  return CacheAccessResult(COLD_MISS);
}

CacheAccessResult
CacheAccessResult::CapacityMiss(const CacheLine &evictedCacheLine) {
  return evictedCacheLine.IsDirty()
             ? CacheAccessResult(evictedCacheLine.GetLastAccessedAddress())
             : CacheAccessResult(CAPACITY_MISS);
}

CacheAccessResult CacheAccessResult::CapacityMiss() {
  return CacheAccessResult(CAPACITY_MISS);
}

CacheAccessResult CacheAccessResult::Discard() {
  return CacheAccessResult(DISCARD);
}

bool CacheAccessResult::IsHit() const { return this->type == HIT; }

bool CacheAccessResult::IsColdMiss() const { return this->type == COLD_MISS; }

bool CacheAccessResult::IsCapacityMiss() const {
  return this->type == CAPACITY_MISS;
}

bool CacheAccessResult::IsDiscard() const { return this->type == DISCARD; }

bool CacheAccessResult::ShouldWriteToSameAddressNextLevel() const {
  return this->writeToSameAddressNextLevel;
}

bool CacheAccessResult::ShouldWriteToEvictedAddressNextLevel() const {
  return this->writeToEvictedAddressNextLevel;
}

CacheAccess CacheAccessResult::CreateCacheWriteAccessToEvictedAddress() const {
  return CacheAccess(false, this->evictedDirtyAddress.value());
}

CacheAccessResult::CacheAccessResult(CacheAccessResult::Type type)
    : type(type), writeToSameAddressNextLevel(false),
      writeToEvictedAddressNextLevel(false) {}

CacheAccessResult::CacheAccessResult(const Address &evictedDirtyAddress)
    : type(CAPACITY_MISS), writeToSameAddressNextLevel(false),
      writeToEvictedAddressNextLevel(true),
      evictedDirtyAddress(evictedDirtyAddress) {}

CacheAccessResult::CacheAccessResult(bool writeToSameAddressNextLevel)
    : type(HIT), writeToSameAddressNextLevel(writeToSameAddressNextLevel),
      writeToEvictedAddressNextLevel(false) {}
