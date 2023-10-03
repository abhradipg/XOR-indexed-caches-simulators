#pragma once

#include <regex>

#include "Settings/CacheSettings.hpp"
#include "Settings/CmdArgParser.hpp"
#include "Util/GlobalVars.hpp"
#include "Util/Util.hpp"

class CacheSettingsParser : public CmdArgParser {
public:
  explicit CacheSettingsParser(std::vector<std::string> cmdTokens);

  [[nodiscard]] std::vector<CacheSettings> ParseCacheSettings() const;

private:
  static constexpr auto cacheSizeConfig = "cache_size";
  static constexpr auto lineSizeConfig = "line_size";
  static constexpr auto associativityConfig = "associativity";
  static constexpr auto replacementPolicyConfig = "replacement_policy";
  static constexpr auto writePolicyConfig = "write_policy";

  [[nodiscard]] std::string ParseCacheConfigFileName() const;

  [[nodiscard]] std::vector<CacheSettings>
  ParseCacheSettingsFromConfigFile(std::ifstream configFile) const;

  void HandleConfigFileLine(std::string line, bool &readingCacheLevel,
                            std::optional<size_t> &foundLineSize,
                            std::vector<CacheSettings> &cacheSettings) const;

  void
  HandleNonEmptyConfigFileLine(std::string line, bool &readingCacheLevel,
                               std::optional<size_t> &foundLineSize,
                               std::vector<CacheSettings> &cacheSettings) const;

  void HandleEmptyConfigFileLine(bool &readingCacheLevel) const;

  [[nodiscard]] std::pair<std::string, std::string>
  ReadConfigNameAndValue(const std::string &line) const;

  [[nodiscard]] bool IsValidConfigName(const std::string &name) const;

  [[nodiscard]] size_t
  ConvertConfigValStrToUnsigned(const std::string &name,
                                const std::string &valStr) const;

  void HandleLineSizeConfig(const std::string &valStr,
                            std::optional<size_t> &foundLineSize) const;

  void HandleConfig(const std::string &name, const std::string &valStr,
                    CacheSettings &currentCacheSettings) const;

  void HandleCacheSizeConfig(const std::string &valStr,
                             CacheSettings &currentCacheSettings) const;

  void HandleAssociativityConfig(const std::string &valStr,
                                 CacheSettings &currentCacheSettings) const;

  void HandleReplacementPolicyConfig(const std::string &valStr,
                                     CacheSettings &currentCacheSettings) const;

  void HandleWritePolicyConfig(const std::string &valStr,
                               CacheSettings &currentCacheSettings) const;

  void ComputeAndSetRemainingCacheSettingsFields(
      size_t lineSize, std::vector<CacheSettings> &cacheSettings) const;
};
