#include "CacheStateFactory.hpp"

std::unique_ptr<CacheState> CacheStateFactory::CreateCacheState(
    const std::vector<CacheSettings> &cacheSettingsVec,
    const GlobalIteratorState &globalIteratorState) {
  assert(!cacheSettingsVec.empty());
  if (cacheSettingsVec.size() == 1)
    return CreateCacheState(cacheSettingsVec[0], globalIteratorState);

  return CreateHierarchicalCacheState(cacheSettingsVec, globalIteratorState);
}

std::unique_ptr<CacheState> CacheStateFactory::CreateCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState) {
  if (cacheSettings.IsFullyAssociative())
    return CacheStateFactory::CreateFullyAssociativeCacheState(
        cacheSettings, globalIteratorState);

  return CacheStateFactory::CreateSetAssociativeCacheState(cacheSettings,
                                                           globalIteratorState);
}

std::unique_ptr<FullyAssociativeCacheState>
CacheStateFactory::CreateFullyAssociativeCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState) {
  assert(cacheSettings.IsFullyAssociative());
  return std::make_unique<FullyAssociativeCacheState>(cacheSettings,
                                                      globalIteratorState);
}

std::unique_ptr<SetAssociativeCacheState>
CacheStateFactory::CreateSetAssociativeCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState) {
  assert(cacheSettings.IsSetAssociative());
  return std::make_unique<SetAssociativeCacheState>(cacheSettings,
                                                    globalIteratorState);
}

std::unique_ptr<HierarchicalCacheState>
CacheStateFactory::CreateHierarchicalCacheState(
    const std::vector<CacheSettings> &cacheSettingsVec,
    const GlobalIteratorState &globalIteratorState) {
  assert(cacheSettingsVec.size() > 1);
  return std::make_unique<HierarchicalCacheState>(cacheSettingsVec,
                                                  globalIteratorState);
}
