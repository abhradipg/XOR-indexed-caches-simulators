#pragma once

#include <Warping/WarpInfo.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

#include "CacheState/HitMissCountByCacheLevel.hpp"
#include "IteratorState/IteratorState.hpp"
#include "Settings/SimulationSettings.hpp"
#include "Util/Timer.hpp"

class SimulationResult {
public:
  explicit SimulationResult(size_t nCacheLevels);

  [[nodiscard]] const HitMissCountByCacheLevel &GetHitMissCount() const;

  void AddSimulatedHit(size_t cacheLevel);
  void AddSimulatedMiss(size_t cacheLevel);

  void UpdateTimerResult(const Timer::TimerResult &result);

  void Warp(const WarpInfo &warpInfo);

  void PrintJSON(const SimulationSettings &simulationSettings,
                 std::ostream &os) const;

private:
  HitMissCountByCacheLevel hitMissCounts;
  std::valarray<size_t> simulatedAccessCounts;
  std::valarray<size_t> warpedAccessCounts;
  Timer::TimerResult timerResult;

  [[nodiscard]] double GetWarpedAccessPercentage(size_t cacheLevel) const;
  [[nodiscard]] double GetNotWarpedAccessPercentage(size_t cacheLevel) const;

  void PrintSimulationSettingsJSON(const SimulationSettings &settings,
                                   std::ostream &os) const;
  void PrintWarpingSettingsJSON(const WarpingSettings &settings,
                                std::ostream &os) const;
  void PrintCacheSettingsJSON(const std::vector<CacheSettings> &settings,
                              std::ostream &os) const;
  void PrintTimerResultsJSON(std::ostream &os) const;
  void PrintAccessCountsJSON(std::ostream &os) const;
  void PrintHitMissCountsJSON(std::ostream &os) const;
  void PrintHitCountsJSON(std::ostream &os) const;
  void PrintMissCountsJSON(std::ostream &os) const;
  void PrintWarpStatsJSON(std::ostream &os) const;
  void PrintWarpedAccessPercentagesJSON(std::ostream &os) const;
  void PrintNotWarpedAccessPercentagesJSON(std::ostream &os) const;
};
