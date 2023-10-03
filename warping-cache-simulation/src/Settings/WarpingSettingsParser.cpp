#include "WarpingSettingsParser.hpp"

WarpingSettingsParser::WarpingSettingsParser(std::vector<std::string> cmdTokens)
    : CmdArgParser(std::move(cmdTokens)) {}

WarpingSettings WarpingSettingsParser::ParseWarpingSettings() const {
  WarpingSettings warpingSettings;
  warpingSettings.tryWarping = this->ParseWarp();
  warpingSettings.useHeuristics = this->ParseHeuristics();
  return warpingSettings;
}

bool WarpingSettingsParser::ParseWarp() const {
  const auto &optValStr = this->GetCmdOptionValue(warpOpt);
  if (optValStr.empty())
    return DEFAULT_TRY_WARPING;
  if (!this->IsValidBoolStr(optValStr))
    this->PrintOptionErrorAndExit(warpOpt);
  return this->ConvertBoolStrToBool(optValStr);
}

bool WarpingSettingsParser::ParseHeuristics() const {
  const auto &optValStr = this->GetCmdOptionValue(heuristicsOpt);
  if (optValStr.empty())
    return DEFAULT_USE_HEURISTICS;
  if (!this->IsValidBoolStr(optValStr))
    this->PrintOptionErrorAndExit(heuristicsOpt);
  return this->ConvertBoolStrToBool(optValStr);
}