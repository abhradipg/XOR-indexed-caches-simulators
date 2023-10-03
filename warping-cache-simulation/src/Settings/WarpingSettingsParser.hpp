#pragma once

#include <vector>

#include "Settings/CmdArgParser.hpp"
#include "Settings/WarpingSettings.hpp"

class WarpingSettingsParser : public CmdArgParser {
public:
  explicit WarpingSettingsParser(std::vector<std::string> cmdTokens);

  [[nodiscard]] WarpingSettings ParseWarpingSettings() const;

private:
  static constexpr auto warpOpt = "-w";
  static constexpr auto heuristicsOpt = "-h";

  [[nodiscard]] bool ParseWarp() const;

  [[nodiscard]] bool ParseHeuristics() const;
};
