#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "Address/Address.hpp"
#include "Address/SymbolicAddress.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Util/IslUtil.hpp"
#include "Util/StrUtil.hpp"
#include "Warping/MemoryBlockMappingChecker.hpp"

class BijectionVerifier {
public:
  BijectionVerifier(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                        &simulationAccessInfos,
                    const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool ShouldCheckCacheBijection(const SnapshotMatch &sm) const;

  [[nodiscard]] bool
  DoesCacheBijectionComplyWithAccessBijection(long furthestItVal,
                                              const SnapshotMatch &sm);

private:
  const IteratorStateMap &iteratorStateMap;

  std::unordered_map<long, std::vector<MemoryBlockMappingChecker>>
      checkersByCoefficient;

  std::unordered_set<long> coefficients;

  void InitCheckers(const std::vector<std::shared_ptr<SimulationAccessInfo>>
                        &simulationAccessInfos);

  void InitCoefficients();

  void RefreshCheckers(long furthestItVal, const SnapshotMatch &sm);

  [[nodiscard]] bool DoesCacheBijectionComplyWithAccessBijection(
      long furthestItVal, const SnapshotMatch &sm, size_t cacheLevel);

  [[nodiscard]] bool IsMappingOkay(size_t itId, const Address &oldAddr,
                                   const Address &newAddr);

  [[nodiscard]] bool IsMappingOkayAccordingToCheckers(
      size_t oldMb, size_t newMb,
      std::vector<MemoryBlockMappingChecker> &checkers) const;
};
