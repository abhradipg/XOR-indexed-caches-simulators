#pragma once

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Util/StrUtil.hpp"

class CmdArgParser {
public:
  CmdArgParser(int argc, char **argv);

protected:
  const std::vector<std::string> cmdTokens;

  explicit CmdArgParser(std::vector<std::string> cmdTokens);

  void PrintErrorAndExit(const std::string &err) const;

  void PrintOptionErrorAndExit(const std::string &optName) const;

  [[nodiscard]] bool IsValidBoolStr(const std::string &str) const;

  [[nodiscard]] bool IsValidUIntStr(const std::string &str) const;

  [[nodiscard]] bool ConvertBoolStrToBool(const std::string &boolStr) const;

  [[nodiscard]] size_t ConvertUIntStrToBool(const std::string &uintStr) const;

  [[nodiscard]] std::string GetCmdOptionValue(const std::string &optName) const;

private:
  static inline const std::vector<
      std::tuple<std::string, std::string, std::string>>
      options = {
          {"-w", "warp", "bool to enable warping, default true"},
          {"-h", "heuristics",
           "bool to enable heuristics for warping, default false"},
          {"-i", "includes",
           "header dirs separated with comma, no spaces, default empty"}};

  void PrintUsage() const;

  void PrintCmdOptions() const;

  [[nodiscard]] bool IsCorrectUsage() const;

  [[nodiscard]] std::vector<std::string> ExtractCmdTokens(int argc,
                                                          char **argv) const;
};
