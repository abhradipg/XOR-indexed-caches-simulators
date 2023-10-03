#pragma once

#include <cassert>
#include <iostream>
#include <regex>
#include <string>

#include "Util/StrUtil.hpp"

class QLRUSettings {
public:
  inline static const std::string regexStr =
      "qlru_h[012][01]_m[0123]_r[012]_u[0123]";

  explicit QLRUSettings(std::string asStr);

  [[nodiscard]] unsigned short GetInsertionAge() const;
  [[nodiscard]] bool IsHitH00() const;
  [[nodiscard]] bool IsHitH01() const;
  [[nodiscard]] bool IsHitH10() const;
  [[nodiscard]] bool IsHitH11() const;
  [[nodiscard]] bool IsHitH20() const;
  [[nodiscard]] bool IsHitH21() const;
  [[nodiscard]] bool IsReplaceR0() const;
  [[nodiscard]] bool IsReplaceR1() const;
  [[nodiscard]] bool IsReplaceR2() const;
  [[nodiscard]] bool IsUpdateU0() const;
  [[nodiscard]] bool IsUpdateU1() const;
  [[nodiscard]] bool IsUpdateU2() const;
  [[nodiscard]] bool IsUpdateU3() const;

  [[nodiscard]] std::string ToString() const;

private:
  const std::string asStr;
  unsigned short insertionAge;
  enum HitFunctionType { H00, H01, H10, H11, H20, H21 } hitFunctionType;
  enum ReplaceFunctionType { R0, R1, R2 } replaceFunctionType;
  enum UpdateFunctionType { U0, U1, U2, U3 } updateFunctionType;

  void InitInsertionAge(const std::string &ageStr);
  void InitHitFunctionType(const std::string &typeStr);
  void InitReplaceFunctionType(const std::string &typeStr);
  void InitUpdateFunctionType(const std::string &typeStr);
};
