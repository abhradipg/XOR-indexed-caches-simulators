#include "ArraySettings.hpp"

bool ArraySettings::DoesHaveStartingAddress(
    const std::string &arrayName) const {
  return this->arrayNameToStartAddress.count(arrayName);
}

bool ArraySettings::DoesHaveDimensionSizes(const std::string &arrayName) const {
  return this->arrayNameToDimSizes.count(arrayName);
}

void ArraySettings::SetArrayStartingAddress(const std::string &arrayName,
                                            size_t startAddress) {
  assert(!this->DoesHaveStartingAddress(arrayName));
  this->arrayNameToStartAddress.emplace(arrayName, startAddress);
}

void ArraySettings::SetArrayDimensionSizes(
    const std::string &arrayName, const std::vector<size_t> &dimSizes) {
  assert(!this->DoesHaveDimensionSizes(arrayName));
  this->arrayNameToDimSizes.emplace(arrayName, dimSizes);
}
