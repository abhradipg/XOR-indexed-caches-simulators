#include "CacheSettingsParser.hpp"

CacheSettingsParser::CacheSettingsParser(std::vector<std::string> cmdTokens)
    : CmdArgParser(std::move(cmdTokens)) {}

std::vector<CacheSettings> CacheSettingsParser::ParseCacheSettings() const {
  const auto &fileName = this->ParseCacheConfigFileName();
  std::ifstream configFile(fileName);
  if (!configFile.good()) {
    std::cerr << "-> Given cache config file " << fileName << " does not exist!"
              << std::endl;
    exit(0);
  }
  if (!configFile.is_open()) {
    std::cerr << "-> Cannot open given cache config file " << fileName << "!"
              << std::endl;
    exit(0);
  }
  return this->ParseCacheSettingsFromConfigFile(std::move(configFile));
}

std::string CacheSettingsParser::ParseCacheConfigFileName() const {
  assert(this->cmdTokens.size() > 1);
  return this->cmdTokens[1];
}

std::vector<CacheSettings>
CacheSettingsParser::ParseCacheSettingsFromConfigFile(
    std::ifstream configFile) const {
  assert(configFile.good() && configFile.is_open());
  std::vector<CacheSettings> res;
  std::optional<size_t> foundLineSize;
  auto readingCacheLevel = false;

  std::string line;
  while (getline(configFile, line))
    this->HandleConfigFileLine(line, readingCacheLevel, foundLineSize, res);

  if (!foundLineSize)
    foundLineSize.emplace(DEFAULT_LINE_SIZE);

  this->ComputeAndSetRemainingCacheSettingsFields(foundLineSize.value(), res);
  return res;
}

void CacheSettingsParser::HandleConfigFileLine(
    std::string line, bool &readingCacheLevel,
    std::optional<size_t> &foundLineSize,
    std::vector<CacheSettings> &cacheSettings) const {
  StrUtil::RemoveSpaces(line);
  StrUtil::ToLower(line);
  if (line.empty())
    this->HandleEmptyConfigFileLine(readingCacheLevel);
  else
    this->HandleNonEmptyConfigFileLine(line, readingCacheLevel, foundLineSize,
                                       cacheSettings);
}

void CacheSettingsParser::HandleNonEmptyConfigFileLine(
    std::string line, bool &readingCacheLevel,
    std::optional<size_t> &foundLineSize,
    std::vector<CacheSettings> &cacheSettings) const {
  const auto isComment = line[0] == '#';
  if (isComment)
    return;

  if (!readingCacheLevel) {
    readingCacheLevel = true;
    cacheSettings.push_back({});
  }

  const auto &[name, valStr] = this->ReadConfigNameAndValue(line);
  if (name == lineSizeConfig)
    this->HandleLineSizeConfig(valStr, foundLineSize);
  else
    this->HandleConfig(name, valStr, cacheSettings.back());
}

void CacheSettingsParser::HandleEmptyConfigFileLine(
    bool &readingCacheLevel) const {
  if (readingCacheLevel)
    readingCacheLevel = false;
}

void CacheSettingsParser::HandleLineSizeConfig(
    const std::string &valStr, std::optional<size_t> &foundLineSize) const {
  const auto val = this->ConvertConfigValStrToUnsigned(lineSizeConfig, valStr);
  if (foundLineSize && foundLineSize.value() != val) {
    std::cerr << "-> Different cache line sizes are not supported!"
              << std::endl;
    exit(0);
  } else
    foundLineSize.emplace(val);
}

void CacheSettingsParser::HandleConfig(
    const std::string &name, const std::string &valStr,
    CacheSettings &currentCacheSettings) const {
  assert(name != lineSizeConfig);
  if (name == cacheSizeConfig)
    this->HandleCacheSizeConfig(valStr, currentCacheSettings);
  else if (name == associativityConfig)
    this->HandleAssociativityConfig(valStr, currentCacheSettings);
  else if (name == replacementPolicyConfig)
    this->HandleReplacementPolicyConfig(valStr, currentCacheSettings);
  else if (name == writePolicyConfig)
    this->HandleWritePolicyConfig(valStr, currentCacheSettings);
}

void CacheSettingsParser::HandleCacheSizeConfig(
    const std::string &valStr, CacheSettings &currentCacheSettings) const {
  const auto val = this->ConvertConfigValStrToUnsigned(cacheSizeConfig, valStr);
  currentCacheSettings.cacheSize = val;
}

void CacheSettingsParser::HandleAssociativityConfig(
    const std::string &valStr, CacheSettings &currentCacheSettings) const {
  const auto val =
      this->ConvertConfigValStrToUnsigned(associativityConfig, valStr);
  currentCacheSettings.associativity = val;
}

void CacheSettingsParser::HandleReplacementPolicyConfig(
    const std::string &valStr, CacheSettings &currentCacheSettings) const {
  CacheSettings::ReplacementPolicy rp;
  if (valStr == "lru")
    rp = CacheSettings::LRU;
  else if (valStr == "plru")
    rp = CacheSettings::PLRU;
  else if (std::regex_match(valStr, std::regex(QLRUSettings::regexStr))) {
    currentCacheSettings.optQLRUSettings.emplace(QLRUSettings(valStr));
    rp = CacheSettings::QLRU;
  } else if (valStr == "lru3plru4")
    rp = CacheSettings::LRU3PLRU4;
  else if (valStr == "fifo")
    rp = CacheSettings::FIFO;
  else {
    std::cerr << "-> Replacement policy " << valStr << " is not supported!"
              << std::endl
              << "-> Supported replacement policies are lru, plru, lru4plru4 "
                 "and fifo!"
              << std::endl;
    exit(0);
  }
  currentCacheSettings.replacementPolicy = rp;
}

void CacheSettingsParser::HandleWritePolicyConfig(
    const std::string &valStr, CacheSettings &currentCacheSettings) const {
  CacheSettings::WritePolicy wr;
  if (valStr == "wb")
    wr = CacheSettings::WRITE_BACK_WRITE_ALLOCATE;
  else if (valStr == "wt")
    wr = CacheSettings::WRITE_THROUGH_NO_WRITE_ALLOCATE;
  else {
    std::cerr << "-> Write policy " << valStr << " is not supported!"
              << std::endl
              << "-> Supported replacement policies are wb and wt!"
              << std::endl;
    exit(0);
  }
  currentCacheSettings.writePolicy = wr;
}

std::pair<std::string, std::string>
CacheSettingsParser::ReadConfigNameAndValue(const std::string &line) const {
  auto delimiterPos = line.find('=');
  if (delimiterPos == std::string::npos) {
    std::cerr << "-> Invalid cache config file!" << std::endl;
    exit(0);
  }

  const auto &name = line.substr(0, delimiterPos);
  if (!this->IsValidConfigName(name)) {
    std::cerr << "-> Invalid cache config " << name << "!" << std::endl;
    exit(0);
  }

  const auto &valStr = line.substr(delimiterPos + 1);
  return {name, valStr};
}

bool CacheSettingsParser::IsValidConfigName(const std::string &name) const {
  return name == cacheSizeConfig || name == lineSizeConfig ||
         name == associativityConfig || name == replacementPolicyConfig ||
         name == writePolicyConfig;
}

size_t CacheSettingsParser::ConvertConfigValStrToUnsigned(
    const std::string &name, const std::string &valStr) const {
  if (!this->IsValidUIntStr(valStr)) {
    std::cerr << "-> Config " << name << " should be an unsigned int!"
              << std::endl;
    exit(0);
  }
  return this->ConvertUIntStrToBool(valStr);
}

void CacheSettingsParser::ComputeAndSetRemainingCacheSettingsFields(
    size_t lineSize, std::vector<CacheSettings> &cacheSettings) const {
  for (size_t cacheLevel = 0; cacheLevel < cacheSettings.size(); cacheLevel++) {
    auto &cs = cacheSettings[cacheLevel];
    cs.cacheLevel = cacheLevel;
    cs.lineSize = lineSize;
    cs.nCacheLines = cs.cacheSize / lineSize;
    cs.nCacheSets = cs.nCacheLines / cs.associativity;
    if (!cs.IsValid()) {
      std::cerr << "-> Invalid cache setting!" << std::endl;
      exit(0);
    }
  }
}