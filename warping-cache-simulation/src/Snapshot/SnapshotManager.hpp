#pragma once

#include <list>
#include <memory>
#include <utility>

#include "CacheState/CacheState.hpp"
#include "Snapshot/Snapshot.hpp"
#include "Snapshot/SnapshotMatch.hpp"

class SnapshotManager {
public:
  explicit SnapshotManager(const IteratorState &iteratorState);

  [[nodiscard]] std::shared_ptr<Snapshot>
  TakeSnapshot(const SimulationResult &simulationResult,
               const CacheState &cacheState) const;

  [[nodiscard]] std::shared_ptr<SnapshotMatch>
  FindMatch(const std::shared_ptr<Snapshot> &snapshot) const;

  void DeleteSnapshots();

  void AddSnapshot(const std::shared_ptr<Snapshot> &snapshot);

private:
  const IteratorState &iteratorState;

  std::unordered_map<size_t, std::shared_ptr<Snapshot>> snapshots;
};
