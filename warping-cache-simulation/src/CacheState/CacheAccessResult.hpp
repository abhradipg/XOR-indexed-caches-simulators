#pragma once

#include <optional>

#include "CacheState/CacheAccess.hpp"
#include "CacheState/CacheLine.hpp"

class CacheAccessResult {
public:
  [[nodiscard]] static CacheAccessResult Hit(bool writeToSameAddressNextLevel);

  [[nodiscard]] static CacheAccessResult ColdMiss();

  [[nodiscard]] static CacheAccessResult
  CapacityMiss(const CacheLine &evictedCacheLine);

  [[nodiscard]] static CacheAccessResult CapacityMiss();

  [[nodiscard]] static CacheAccessResult Discard();

  [[nodiscard]] bool IsHit() const;

  [[nodiscard]] bool IsColdMiss() const;

  [[nodiscard]] bool IsCapacityMiss() const;

  [[nodiscard]] bool IsDiscard() const;

  [[nodiscard]] bool ShouldWriteToSameAddressNextLevel() const;

  [[nodiscard]] bool ShouldWriteToEvictedAddressNextLevel() const;

  [[nodiscard]] CacheAccess CreateCacheWriteAccessToEvictedAddress() const;

private:
  const enum Type { HIT, COLD_MISS, CAPACITY_MISS, DISCARD } type;

  const bool writeToSameAddressNextLevel;

  const bool writeToEvictedAddressNextLevel;

  const std::optional<Address> evictedDirtyAddress;

  explicit CacheAccessResult(Type type);

  explicit CacheAccessResult(const Address &evictedDirtyAddress);

  explicit CacheAccessResult(bool writeToSameAddressNextLevel);
};
