#pragma once

#include <string>
#include <vector>

#include "Settings/CacheSettings.hpp"
#include "WarpingSettings.hpp"

struct SimulationSettings {
public:
  std::string source;

  std::vector<std::string> includes;

  std::vector<CacheSettings> cacheSettings;

  WarpingSettings warpingSettings;
};
