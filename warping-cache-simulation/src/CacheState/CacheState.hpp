#pragma once

#include "CacheState/CacheAccess.hpp"
#include "CacheState/CacheAccessResult.hpp"
#include "CacheState/HitMissCountByCacheLevel.hpp"
#include "Simulation/SimulationResult.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"
#include "Warping/WarpInfo.hpp"

class CacheState {
public:
  explicit CacheState(size_t cacheLevel);

  virtual ~CacheState() = default;

  CacheState(size_t cacheLevel, size_t nCacheLevels);

  void ProcessAccesses(const std::vector<CacheAccess> &accesses,
                       SimulationResult &simulationResult);

  [[nodiscard]] size_t GetNumberOfEmptyLines() const;

  [[nodiscard]] size_t GetNumberOfCacheLevels() const;

  [[nodiscard]] virtual CacheAccessResult
  ProcessAccess(const CacheAccess &access,
                SimulationResult &simulationResult) = 0;

  [[nodiscard]] virtual size_t GetNumberOfLines() const = 0;

  [[nodiscard]] virtual size_t GetNumberOfNonEmptyLines() const = 0;

  virtual void AddLinesToCacheStateSnapshot(
      std::vector<CacheStateSnapshot> &cssByCacheLevel) const = 0;

  virtual void Warp(const WarpInfo &warpInfo) = 0;

protected:
  const size_t cacheLevel;

  const size_t nCacheLevels;
};
