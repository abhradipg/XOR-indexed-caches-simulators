#pragma once

#include <isl/cpp.h>
#include <string>

#include "Address/SymbolicAddress.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Simulation/SimulationAccessInfo.hpp"

#define APPEND1 "_c1_"
#define APPEND2 "_c2_"

class CoefficientConflict {
public:
  CoefficientConflict(const SimulationAccessInfo &sai1,
                      const SimulationAccessInfo &sai2,
                      const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool IsEmpty() const;

  [[nodiscard]] size_t
  ComputeIterationsUntilNextConflict(const SnapshotMatch &snapshotMatch) const;

private:
  std::string conflictSetStringBasis;

  isl::basic_set conflictSetBasis;

  isl::ctx context;

  const IteratorStateMap &iteratorStateMap;

  void InitConflictSetStringBasis(const SimulationAccessInfo &sai1,
                                  const SimulationAccessInfo &sai2);

  void InitConflictSetBasis();

  [[nodiscard]] std::string
  ConstructNextConflictQueryStr(const SnapshotMatch &snapshotMatch) const;
};
