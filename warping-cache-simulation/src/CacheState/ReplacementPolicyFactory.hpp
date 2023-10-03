#pragma once

#include <memory>

#include "CacheState/FIFOReplacementPolicy.hpp"
#include "CacheState/LRU3PLRU4ReplacementPolicy.hpp"
#include "CacheState/LRUReplacementPolicy.hpp"
#include "CacheState/PLRUReplacementPolicy.hpp"
#include "CacheState/QLRUReplacementPolicy.hpp"
#include "CacheState/ReplacementPolicy.hpp"
#include "Settings/CacheSettings.hpp"

namespace ReplacementPolicyFactory {

[[nodiscard]] std::unique_ptr<ReplacementPolicy>
CreateReplacementPolicy(const CacheSettings &cacheSettings,
                        const GlobalIteratorState &globalIteratorState);

}
