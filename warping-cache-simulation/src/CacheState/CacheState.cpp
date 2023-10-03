#include "CacheState.hpp"

CacheState::CacheState(size_t cacheLevel)
    : cacheLevel(cacheLevel), nCacheLevels(1) {}

CacheState::CacheState(size_t cacheLevel, size_t nCacheLevels)
    : cacheLevel(cacheLevel), nCacheLevels(nCacheLevels) {}

void CacheState::ProcessAccesses(const std::vector<CacheAccess> &accesses,
                                 SimulationResult &simulationResult) {
  for (const auto &access : accesses)
    (void)this->ProcessAccess(access, simulationResult);
}

size_t CacheState::GetNumberOfEmptyLines() const {
  return this->GetNumberOfLines() - this->GetNumberOfNonEmptyLines();
}

size_t CacheState::GetNumberOfCacheLevels() const { return this->nCacheLevels; }