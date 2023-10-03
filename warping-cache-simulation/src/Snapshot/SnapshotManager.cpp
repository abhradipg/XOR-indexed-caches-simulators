#include "SnapshotManager.hpp"

SnapshotManager::SnapshotManager(const IteratorState &iteratorState)
    : iteratorState(iteratorState) {}

std::shared_ptr<Snapshot>
SnapshotManager::TakeSnapshot(const SimulationResult &simulationResult,
                              const CacheState &cacheState) const {
  return std::make_shared<Snapshot>(this->iteratorState, simulationResult,
                                    cacheState);
}

std::shared_ptr<SnapshotMatch>
SnapshotManager::FindMatch(const std::shared_ptr<Snapshot> &snapshot) const {
  const auto it = this->snapshots.find(snapshot->GetSymbolicHash());
  if (it == this->snapshots.end())
    return nullptr;

  if (!it->second->IsSymbolicallyEqual(*snapshot))
    return nullptr;

  return std::make_shared<SnapshotMatch>(it->second, snapshot);
}

void SnapshotManager::DeleteSnapshots() { this->snapshots.clear(); }

void SnapshotManager::AddSnapshot(const std::shared_ptr<Snapshot> &snapshot) {
  this->snapshots[snapshot->GetSymbolicHash()] = snapshot;
}
