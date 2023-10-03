#pragma once

#include "CacheAccess.hpp"
#include "CacheState/CacheLine.hpp"
#include "CacheState/CacheState.hpp"
#include "CacheState/FIFOReplacementPolicy.hpp"
#include "CacheState/LRUReplacementPolicy.hpp"
#include "CacheState/PLRUReplacementPolicy.hpp"
#include "CacheState/ReplacementPolicy.hpp"
#include "CacheState/ReplacementPolicyFactory.hpp"
#include "Settings/CacheSettings.hpp"
#include "Snapshot/CacheStateSnapshot.hpp"

class CacheAccess;

class FullyAssociativeCacheState : public CacheState {
public:
  FullyAssociativeCacheState(const CacheSettings &cacheSettings,
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

  const std::unique_ptr<ReplacementPolicy> replacementPolicy;

  const CacheSettings::WritePolicy writePolicy;

  [[nodiscard]] bool IsHit(const CacheAccess &access) const;

  [[nodiscard]] CacheAccessResult ProcessHit(const CacheAccess &access);
  [[nodiscard]] CacheAccessResult ProcessMiss(const CacheAccess &access);
  [[nodiscard]] CacheAccessResult ProcessReadHit(const Address &address);
  [[nodiscard]] CacheAccessResult ProcessReadMiss(const Address &address);
  [[nodiscard]] CacheAccessResult ProcessWriteHit(const Address &address);
  [[nodiscard]] CacheAccessResult ProcessWriteMiss(const Address &address);

  void OnHit(const CacheAccess &access);
  void OnMiss(const CacheAccess &access);
  void OnReadHit(const Address &address);
  void OnReadMiss(const Address &address);
  void OnWriteHit(const Address &address);
  void OnWriteMiss(const Address &address);

  [[nodiscard]] bool IsWriteBack() const;
  [[nodiscard]] bool IsWriteThrough() const;
  [[nodiscard]] bool IsWriteAllocate() const;
  [[nodiscard]] bool IsNoWriteAllocate() const;
};
