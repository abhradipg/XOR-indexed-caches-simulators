#pragma once

#include <isl/cpp.h>
#include <memory>
#include <vector>

#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/SimulationAccessInfo.hpp"
#include "Snapshot/SnapshotMatch.hpp"
#include "Util/GlobalVars.hpp"

class MemoryBlockMappingChecker {
public:
  MemoryBlockMappingChecker(const SimulationAccessInfo &sai,
                            const IteratorStateMap &ism);

  void Refresh(long furthestItVal, const SnapshotMatch &sm);

  [[nodiscard]] bool IsOkay(size_t left, size_t right) const;

private:
  const IteratorStateMap &iteratorStateMap;
  const SimulationAccessInfo &sai;

  const isl::basic_map mbRel;
  const isl::multi_aff mbRelDomainIdentity;
  const isl::multi_aff mbRelRangeIdentity;

  std::optional<long> mbDelta;
  std::optional<isl::basic_set> restrictedMbDomain;

  [[nodiscard]] bool IsInAccessedArrayMbInterval(size_t mb) const;

  [[nodiscard]] bool IsInRestrictedMbDomain(size_t mb) const;

  [[nodiscard]] bool IsInRestrictedMbRange(size_t mb) const;

  void RefreshMbDelta(const SnapshotMatch &sm);

  void RefreshRestrictedMbDomain(long furthestItVal, const SnapshotMatch &sm);

  [[nodiscard]] long ComputeMbDelta(const SnapshotMatch &sm) const;

  [[nodiscard]] isl::basic_set
  ComputeRestrictedMbDomain(long furthestItVal, const SnapshotMatch &sm) const;

  [[nodiscard]] isl::basic_map
  ComputeRestrictedMbRel(long furthestItVal, const SnapshotMatch &sm) const;

  [[nodiscard]] isl::basic_map
  RestrictRelWithOuterIts(const isl::basic_map &rel) const;

  [[nodiscard]] isl::basic_map
  RestrictRelWithChangingIt(const isl::basic_map &rel, long furthestItVal,
                            const SnapshotMatch &sm) const;
};
