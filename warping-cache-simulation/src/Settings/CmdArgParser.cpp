#include "CmdArgParser.hpp"

CmdArgParser::CmdArgParser(int argc, char **argv)
    : cmdTokens(this->ExtractCmdTokens(argc, argv)) {
  if (!this->IsCorrectUsage()) {
    this->PrintUsage();
    exit(0);
  }
}

CmdArgParser::CmdArgParser(std::vector<std::string> cmdTokens)
    : cmdTokens(std::move(cmdTokens)) {}

void CmdArgParser::PrintErrorAndExit(const std::string &err) const {
  std::cerr << err << std::endl;
  exit(0);
}

void CmdArgParser::PrintOptionErrorAndExit(const std::string &optName) const {
  this->PrintErrorAndExit("-> Option " + optName + " has an invalid value!");
}

bool CmdArgParser::IsValidBoolStr(const std::string &str) const {
  return str == "true" || str == "false";
}

bool CmdArgParser::IsValidUIntStr(const std::string &str) const {
  return StrUtil::IsUnsignedInteger(str);
}

bool CmdArgParser::ConvertBoolStrToBool(const std::string &boolStr) const {
  return boolStr == "true";
}

size_t CmdArgParser::ConvertUIntStrToBool(const std::string &uintStr) const {
  return std::stoul(uintStr);
}

std::vector<std::string> CmdArgParser::ExtractCmdTokens(int argc,
                                                        char **argv) const {
  std::vector<std::string> res;
  for (int i = 1; i < argc; i++)
    res.emplace_back(argv[i]);
  return res;
}

std::string CmdArgParser::GetCmdOptionValue(const std::string &optName) const {
  auto it = std::find(this->cmdTokens.begin(), this->cmdTokens.end(), optName);
  if (it != this->cmdTokens.end() && std::next(it) != this->cmdTokens.end())
    return *std::next(it);
  return "";
}

bool CmdArgParser::IsCorrectUsage() const {
  if (this->cmdTokens.size() < 2) {
    std::cerr << "-> No source file or config file!" << std::endl;
    return false;
  }

  auto it = std::next(this->cmdTokens.begin(), 2);
  while (it != this->cmdTokens.end()) {
    auto nextIt = std::next(it);
    if (nextIt == this->cmdTokens.end())
      return false;

    const auto optIt = find_if(
        CmdArgParser::options.begin(), CmdArgParser::options.end(),
        [it](const std::tuple<std::string, std::string, std::string> &opt) {
          return std::get<0>(opt) == *it;
        });

    if (optIt == CmdArgParser::options.end())
      return false;

    it += 2;
  }

  return true;
}

void CmdArgParser::PrintUsage() const {
  std::cout << "How to Run" << std::endl
            << "\t ./warping-cache-simulation <source_file> <cache_config_file>"
            << std::endl
            << std::endl
            << "Additional Options" << std::endl;
  this->PrintCmdOptions();
}

void CmdArgParser::PrintCmdOptions() const {
  for (const auto &el : CmdArgParser::options) {
    const auto &[name, desc, help] = el;
    std::cout << "\t" << std::left << std::setw(5) << name << "\t"
              << std::setw(20) << desc << "\t" << help << std::endl;
  }
  std::cout << std::endl;
}
