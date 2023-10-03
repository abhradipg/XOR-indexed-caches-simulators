#include "QLRUSettings.hpp"

#include <utility>

QLRUSettings::QLRUSettings(std::string asStr) : asStr(std::move(asStr)) {
  assert(std::regex_match(this->asStr, std::regex(QLRUSettings::regexStr)));
  const auto settingsStr = this->asStr.substr(5);
  assert(!settingsStr.empty());
  const auto &settingsVec = StrUtil::SplitStringByChar(settingsStr, '_');
  assert(settingsVec.size() == 4);
  this->InitHitFunctionType(settingsVec[0]);
  this->InitInsertionAge(settingsVec[1]);
  this->InitReplaceFunctionType(settingsVec[2]);
  this->InitUpdateFunctionType(settingsVec[3]);
}

void QLRUSettings::InitInsertionAge(const std::string &ageStr) {
  assert(ageStr[0] == 'm' && ageStr.size() == 2);
  this->insertionAge = std::stoul(ageStr.substr(1));
}

void QLRUSettings::InitHitFunctionType(const std::string &typeStr) {
  if (typeStr == "h00")
    this->hitFunctionType = H00;
  else if (typeStr == "h01")
    this->hitFunctionType = H01;
  else if (typeStr == "h10")
    this->hitFunctionType = H10;
  else if (typeStr == "h11")
    this->hitFunctionType = H11;
  else if (typeStr == "h20")
    this->hitFunctionType = H20;
  else if (typeStr == "h21")
    this->hitFunctionType = H21;
  else {
    std::cerr
        << "-> Invalid QLRU hit function type " << typeStr << "!" << std::endl
        << "-> Possible hit function types are H00, H01, H10, H11, H20 and H21!"
        << std::endl;
    exit(0);
  }
}

void QLRUSettings::InitReplaceFunctionType(const std::string &typeStr) {
  if (typeStr == "r0")
    this->replaceFunctionType = R0;
  else if (typeStr == "r1")
    this->replaceFunctionType = R1;
  else if (typeStr == "r2")
    this->replaceFunctionType = R2;
  else {
    std::cerr << "-> Invalid QLRU replace function type " << typeStr << "!"
              << std::endl
              << "   Possible replace function types are R0, R1 and R2!"
              << std::endl;
    exit(0);
  }
}

void QLRUSettings::InitUpdateFunctionType(const std::string &typeStr) {
  if (typeStr == "u0")
    this->updateFunctionType = U0;
  else if (typeStr == "u1")
    this->updateFunctionType = U1;
  else if (typeStr == "u2")
    this->updateFunctionType = U2;
  else if (typeStr == "u3")
    this->updateFunctionType = U3;
  else {
    std::cerr << "-> Invalid QLRU update function type " << typeStr << "!"
              << std::endl
              << "   Possible update function types are U0, U1, U2 and U3!"
              << std::endl;
    exit(0);
  }
}

unsigned short QLRUSettings::GetInsertionAge() const {
  return this->insertionAge;
}

bool QLRUSettings::IsHitH00() const { return this->hitFunctionType == H00; }

bool QLRUSettings::IsHitH01() const { return this->hitFunctionType == H01; }

bool QLRUSettings::IsHitH10() const { return this->hitFunctionType == H10; }

bool QLRUSettings::IsHitH11() const { return this->hitFunctionType == H11; }

bool QLRUSettings::IsHitH20() const { return this->hitFunctionType == H20; }

bool QLRUSettings::IsHitH21() const { return this->hitFunctionType == H21; }

bool QLRUSettings::IsReplaceR0() const {
  return this->replaceFunctionType == R0;
}
bool QLRUSettings::IsReplaceR1() const {
  return this->replaceFunctionType == R1;
}
bool QLRUSettings::IsReplaceR2() const {
  return this->replaceFunctionType == R2;
}

bool QLRUSettings::IsUpdateU0() const { return this->updateFunctionType == U0; }

bool QLRUSettings::IsUpdateU1() const { return this->updateFunctionType == U1; }

bool QLRUSettings::IsUpdateU2() const { return this->updateFunctionType == U2; }

bool QLRUSettings::IsUpdateU3() const { return this->updateFunctionType == U3; }

std::string QLRUSettings::ToString() const { return this->asStr; }
