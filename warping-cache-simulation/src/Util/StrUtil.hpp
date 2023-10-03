#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdarg>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace StrUtil {

[[nodiscard]] bool HasWhiteSpaces(const std::string &str);

void RemoveSpaces(std::string &str);

void ToLower(std::string &str);

[[nodiscard]] std::string ReplaceAllOccurrences(std::string str,
                                                const std::string &subStr,
                                                const std::string &newSubStr);

[[nodiscard]] std::string ReplaceFirstOccurrence(std::string str,
                                                 const std::string &subStr,
                                                 const std::string &newSubStr);

[[nodiscard]] std::vector<std::string> SplitStringByChar(const std::string &str,
                                                         char c);

[[nodiscard]] bool IsInteger(const std::string &str);

[[nodiscard]] bool IsUnsignedInteger(const std::string &str);

[[nodiscard]] bool IsOuterIteratorName(const std::string &inner,
                                       const std::string &outer);

struct LexLess {
  bool operator()(const std::string &str1, const std::string &str2) const;
};

template <typename... Args>
std::string Format(const std::string &format, Args... args) {
  // https://stackoverflow.com/a/26221725
  size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
  if (size <= 0)
    throw std::runtime_error("Failed StrUtil::Format!");
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

} // namespace StrUtil
