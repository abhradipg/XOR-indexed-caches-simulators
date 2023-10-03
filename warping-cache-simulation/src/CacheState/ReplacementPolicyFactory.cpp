#include "ReplacementPolicyFactory.hpp"

std::unique_ptr<ReplacementPolicy>
ReplacementPolicyFactory::CreateReplacementPolicy(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState) {
  if (cacheSettings.IsLRU())
    return std::make_unique<LRUReplacementPolicy>(cacheSettings.nCacheLines,
                                                  globalIteratorState);
  else if (cacheSettings.IsPLRU())
    return std::make_unique<PLRUReplacementPolicy>(cacheSettings.nCacheLines,
                                                   globalIteratorState);
  else if (cacheSettings.IsQLRU()) {
    assert(cacheSettings.optQLRUSettings);
    return std::make_unique<QLRUReplacementPolicy>(
        cacheSettings.nCacheLines, globalIteratorState,
        cacheSettings.optQLRUSettings.value());

  } else if (cacheSettings.IsFIFO())
    return std::make_unique<FIFOReplacementPolicy>(cacheSettings.nCacheLines,
                                                   globalIteratorState);
  else if (cacheSettings.IsLRU3PLRU4())
    return std::make_unique<LRU3PLRU4ReplacementPolicy>(
        cacheSettings.nCacheLines, globalIteratorState);
  assert(false);
  return nullptr;
}
