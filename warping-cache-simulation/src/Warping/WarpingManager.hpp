#pragma once

#include <memory>

#include "CacheState/CacheState.hpp"
#include "FurthestWarpingPointManager.hpp"
#include "IteratorState/IteratorState.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Settings/WarpingSettings.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Simulation/SimulationResult.hpp"
#include "Snapshot/SnapshotManager.hpp"
#include "Warping/FurthestWarpingPointManager.hpp"
#include "Warping/WarpInfo.hpp"
#include "Warping/WarpingPeriodCalculator.hpp"

class WarpingManager {
public:
  WarpingManager(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                     &simulationAccessInfos,
                 const IteratorStateMap &iteratorStateMap);

  virtual ~WarpingManager(){};

  void TryWarping(CacheState &cacheState, SimulationResult &simulationResult);

  virtual void OnIteratorFinish() = 0;

protected:
  const IteratorStateMap &iteratorStateMap;
  const std::shared_ptr<IteratorState> &iteratorState;

  const size_t warpingPeriod;

  virtual void OnInitialWarpTry(const CacheState &cacheState) = 0;

  virtual void OnSuccessfulInitialWarp(const WarpInfo &warpInfo) = 0;

  virtual void OnSuccessfulWarp(const WarpInfo &warpInfo) = 0;

  virtual void OnUnsuccessfulInitialWarp() = 0;

  [[nodiscard]] virtual bool
  ShouldTryWarping(const CacheState &cacheState) const = 0;

  [[nodiscard]] bool IsItTimeToWarpAccordingToPeriod() const;

  void ResetSnapshots();

private:
  SnapshotManager snapshotManager;
  FurthestWarpingPointManager furthestWarpingPointManager;

  [[nodiscard]] std::shared_ptr<SnapshotMatch>
  SaveSnapshotAndFindMatch(const CacheState &cacheState,
                           const SimulationResult &simulationResult);

  void TryWarpingAccordingToSnapshotMatch(
      const std::shared_ptr<SnapshotMatch> &snapshotMatch,
      CacheState &cacheState, SimulationResult &simulationResult);

  [[nodiscard]] size_t
  TryInitialWarpingAndGetNWarp(const SnapshotMatch &snapshotMatch,
                               CacheState &cacheState,
                               SimulationResult &simulationResult);

  [[nodiscard]] size_t
  TryWarpingAgainAndGetNWarp(const SnapshotMatch &lastSnapshotMatch,
                             CacheState &cacheState,
                             SimulationResult &simulationResult);

  void TryKeepWarping(size_t lastNWarp,
                      const std::shared_ptr<SnapshotMatch> &lastSnapshotMatch,
                      CacheState &cacheState,
                      SimulationResult &simulationResult);

  void NormalizeSnapshotManager(
      size_t lastNWarp,
      const std::shared_ptr<SnapshotMatch> &lastSnapshotMatch);

  [[nodiscard]] size_t ComputeNWarp(const SnapshotMatch &snapshotMatch);

  [[nodiscard]] size_t
  ComputeIterationsToWarp(const SnapshotMatch &snapshotMatch);

  void InitialWarp(const WarpInfo &warpInfo, CacheState &cacheState,
                   SimulationResult &simulationResult);

  void Warp(const WarpInfo &warpInfo, CacheState &cacheState,
            SimulationResult &simulationResult);

  [[nodiscard]] WarpInfo
  CreateWarpInfo(size_t nWarp, const SnapshotMatch &snapshotMatch) const;
};
