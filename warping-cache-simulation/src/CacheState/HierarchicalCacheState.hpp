#pragma once

#include <memory>
#include <vector>

#include "CacheState/CacheState.hpp"
#include "CacheState/CacheStateFactory.hpp"
#include "Settings/CacheSettings.hpp"

class HierarchicalCacheState : public CacheState {
public:
  HierarchicalCacheState(const std::vector<CacheSettings> &cacheSettingsVec,
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
  const std::vector<std::unique_ptr<CacheState>> cacheStates;

  const size_t nLines;

  void
  ProcessAccessOnCacheLevelsRecursively(size_t cacheLevel,
                                        const CacheAccess &access,
                                        SimulationResult &simulationResult);

  void HandleWriteBackOnCacheLevel(size_t cacheLevel,
                                   const CacheAccessResult &accessResult,
                                   SimulationResult &simulationResult);

  void HandleWriteThroughCacheLevel(size_t cacheLevel,
                                    const CacheAccess &access,
                                    const CacheAccessResult &accessResult,
                                    SimulationResult &simulationResult);

  [[nodiscard]] std::vector<std::unique_ptr<CacheState>>
  CreateCacheStates(const std::vector<CacheSettings> &cacheSettingsVec,
                    const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] size_t ExtractNumberOfLines() const;
};
