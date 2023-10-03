#pragma once

#include "Settings/CacheSettingsParser.hpp"
#include "Settings/CmdArgParser.hpp"
#include "Settings/SimulationSettings.hpp"
#include "Settings/WarpingSettingsParser.hpp"

class SimulationSettingsParser : public CmdArgParser {
public:
  SimulationSettingsParser(int argc, char **argv);

  [[nodiscard]] SimulationSettings ParseSimulationSettings() const;

private:
  static constexpr auto includesOpt = "-i";

  const WarpingSettingsParser warpingSettingsParser;
  const CacheSettingsParser cacheSettingsParser;

  [[nodiscard]] std::string ParseSourceFile() const;

  [[nodiscard]] std::vector<std::string> ParseIncludes() const;
};
