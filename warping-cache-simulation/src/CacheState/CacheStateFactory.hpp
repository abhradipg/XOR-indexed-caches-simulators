#pragma once

#include <memory>

#include "CacheState/CacheState.hpp"
#include "CacheState/FullyAssociativeCacheState.hpp"
#include "CacheState/HierarchicalCacheState.hpp"
#include "CacheState/SetAssociativeCacheState.hpp"
#include "IteratorState/GlobalIteratorState.hpp"
#include "Settings/CacheSettings.hpp"

// Forward declarations
class SetAssociativeCacheState;
class HierarchicalCacheState;

namespace CacheStateFactory {

[[nodiscard]] std::unique_ptr<CacheState>
CreateCacheState(const std::vector<CacheSettings> &cacheSettingsVec,
                 const GlobalIteratorState &globalIteratorState);

[[nodiscard]] std::unique_ptr<CacheState>
CreateCacheState(const CacheSettings &cacheSettings,
                 const GlobalIteratorState &globalIteratorState);

[[nodiscard]] std::unique_ptr<FullyAssociativeCacheState>
CreateFullyAssociativeCacheState(
    const CacheSettings &cacheSettings,
    const GlobalIteratorState &globalIteratorState);

[[nodiscard]] std::unique_ptr<SetAssociativeCacheState>
CreateSetAssociativeCacheState(const CacheSettings &cacheSettings,
                               const GlobalIteratorState &globalIteratorState);

[[nodiscard]] std::unique_ptr<HierarchicalCacheState>
CreateHierarchicalCacheState(const std::vector<CacheSettings> &cacheSettingsVec,
                             const GlobalIteratorState &globalIteratorState);

} // namespace CacheStateFactory
