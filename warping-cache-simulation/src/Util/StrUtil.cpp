#include "StrUtil.hpp"

void StrUtil::RemoveSpaces(std::string &str) {
  str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
}

void StrUtil::ToLower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

std::string StrUtil::ReplaceAllOccurrences(std::string str,
                                           const std::string &subStr,
                                           const std::string &newSubStr) {
  size_t pos = 0;
  while ((pos = str.find(subStr, pos)) != std::string::npos) {
    str.replace(pos, subStr.length(), newSubStr);
    pos += newSubStr.length();
  }
  return str;
}

std::string StrUtil::ReplaceFirstOccurrence(std::string str,
                                            const std::string &subStr,
                                            const std::string &newSubStr) {
  size_t pos = str.find(subStr);
  assert(pos != std::string::npos);
  str.replace(pos, subStr.length(), newSubStr);
  return str;
}

std::vector<std::string> StrUtil::SplitStringByChar(const std::string &str,
                                                    char c) {
  std::vector<std::string> parts;
  std::stringstream ss(str);
  std::string part;
  while (std::getline(ss, part, c))
    parts.push_back(part);
  return parts;
}

bool StrUtil::IsInteger(const std::string &str) {
  if (str[0] == '-' && str.length() > 1)
    return str.find_first_not_of("0123456789", 1) == std::string::npos;
  return str.find_first_not_of("0123456789") == std::string::npos;
}

bool StrUtil::IsUnsignedInteger(const std::string &str) {
  return str.find_first_not_of("0123456789") == std::string::npos;
}

bool StrUtil::IsOuterIteratorName(const std::string &inner,
                                  const std::string &outer) {
  assert(inner[0] == 'i' && inner.length() > 1);
  assert(outer[0] == 'i' && outer.length() > 1);
  const auto &innerNumStr = inner.substr(1);
  const auto &outerNumStr = outer.substr(1);
  assert(StrUtil::IsInteger(innerNumStr) && StrUtil::IsInteger(outerNumStr));
  return std::stol(innerNumStr) > std::stol(outerNumStr);
}

bool StrUtil::LexLess::operator()(const std::string &str1,
                                  const std::string &str2) const {
  if (str1.length() < str2.length())
    return true;
  return str1 < str2;
}