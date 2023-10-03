#include "SimulationSettingsParser.hpp"

SimulationSettingsParser::SimulationSettingsParser(int argc, char **argv)
    : CmdArgParser(argc, argv), warpingSettingsParser(this->cmdTokens),
      cacheSettingsParser(this->cmdTokens) {}

SimulationSettings SimulationSettingsParser::ParseSimulationSettings() const {
  SimulationSettings simulationSettings;
  simulationSettings.source = this->ParseSourceFile();
  simulationSettings.includes = this->ParseIncludes();
  simulationSettings.warpingSettings =
      this->warpingSettingsParser.ParseWarpingSettings();
  simulationSettings.cacheSettings =
      this->cacheSettingsParser.ParseCacheSettings();
  return simulationSettings;
}

std::string SimulationSettingsParser::ParseSourceFile() const {
  const auto sourceFile = this->cmdTokens[0];
  if (!Util::DoesFileExist(sourceFile)) {
    std::cerr << "-> Given source file " << sourceFile << " does not exist!"
              << std::endl;
    exit(0);
  }
  return sourceFile;
}

std::vector<std::string> SimulationSettingsParser::ParseIncludes() const {
  const auto &optStr = this->GetCmdOptionValue(includesOpt);
  if (optStr.empty())
    return {};
  return StrUtil::SplitStringByChar(optStr, ',');
}
