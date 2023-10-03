#pragma once

#include <cassert>
#include <cstddef>

#include "Settings/SimulationSettings.hpp"
#include "Util/Util.hpp"

namespace GlobalVars {

extern size_t lineSize;
extern size_t nCacheLevels;
extern std::vector<size_t> nCacheSetsByCacheLevel;
extern std::vector<size_t> nCacheLinesByCacheLevel;
extern size_t nTotalCacheLines;

void InitGlobalVars(const SimulationSettings &simulationSettings);

} // namespace GlobalVars
