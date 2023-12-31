#pragma once

#include <algorithm>
#include <cassert>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "CacheAccess.hpp"
#include "CacheState/CacheLine.hpp"
#include "CacheState/CacheState.hpp"
#include "CacheState/CacheStateFactory.hpp"
#include "CacheState/FIFOReplacementPolicy.hpp"
#include "CacheState/FullyAssociativeCacheState.hpp"
#include "CacheState/LRUReplacementPolicy.hpp"
#include "CacheState/PLRUReplacementPolicy.hpp"
#include "HitMissCountByCacheLevel.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Settings/CacheSettings.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"
#include "Util/GlobalVars.hpp"
#include "Util/StrUtil.hpp"

class FullyAssociativeCacheState;

class SetAssociativeCacheState : public CacheState {
public:
  SetAssociativeCacheState(const CacheSettings &cacheSettings,
                           const GlobalIteratorState &globalIteratorState);

  [[nodiscard]] CacheAccessResult
  ProcessAccess(const CacheAccess &access,
                SimulationResult &simulationResult) override;

  [[nodiscard]] size_t GetNumberOfLines() const override;

  [[nodiscard]] size_t GetNumberOfNonEmptyLines() const override;

  void AddLinesToCacheStateSnapshot(
      std::vector<CacheStateSnapshot> &cssByCacheLevel) const override;

  void Warp(const WarpInfo &warpInfo) override;

private:
  const size_t nLines;
  const size_t nSets;

  std::vector<std::unique_ptr<FullyAssociativeCacheState>> cacheSetStates;

  size_t lastAccessedSetIndex = 0;

  void WarpCacheSets(const WarpInfo &warpInfo);

  void RotateCacheSets(size_t nRightRotations);

  void RotateCacheSetsRight(size_t nRotations);

  void RotateCacheSetsLeft(size_t nRotations);

  int clog2(size_t x) const;

  [[nodiscard]] size_t ComputeSetIndex(const CacheAccess &access) const;

  [[nodiscard]] std::vector<std::unique_ptr<FullyAssociativeCacheState>>
  CreateCacheSetStates(const CacheSettings &cacheSettings,
                       const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] CacheSettings
  CreateCacheSetSettings(const CacheSettings &cacheSettings) const;
};
