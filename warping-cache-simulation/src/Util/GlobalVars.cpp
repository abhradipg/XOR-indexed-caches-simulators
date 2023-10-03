#include "GlobalVars.hpp"

size_t GlobalVars::lineSize;
size_t GlobalVars::nCacheLevels;
std::vector<size_t> GlobalVars::nCacheSetsByCacheLevel;
std::vector<size_t> GlobalVars::nCacheLinesByCacheLevel;
size_t GlobalVars::nTotalCacheLines;

void GlobalVars::InitGlobalVars(const SimulationSettings &simulationSettings) {
  nCacheLevels = simulationSettings.cacheSettings.size();
  assert(nCacheLevels > 0);
  lineSize = simulationSettings.cacheSettings[0].lineSize;
  nCacheSetsByCacheLevel.reserve(nCacheLevels);
  nCacheLinesByCacheLevel.reserve(nCacheLevels);
  nTotalCacheLines = 0;
  for (const auto &cs : simulationSettings.cacheSettings) {
    assert(lineSize == cs.lineSize);
    nCacheSetsByCacheLevel.push_back(cs.nCacheSets);
    nCacheLinesByCacheLevel.push_back(cs.nCacheLines);
    nTotalCacheLines += cs.nCacheLines;
  }
}