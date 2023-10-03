#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class ArraySettings {
  // This class is currently not used.
public:
  [[nodiscard]] bool
  DoesHaveStartingAddress(const std::string &arrayName) const;

  [[nodiscard]] bool DoesHaveDimensionSizes(const std::string &arrayName) const;

  void SetArrayStartingAddress(const std::string &arrayName,
                               size_t startAddress);

  void SetArrayDimensionSizes(const std::string &arrayName,
                              const std::vector<size_t> &dimSizes);

private:
  std::unordered_map<std::string, size_t> arrayNameToStartAddress;
  std::unordered_map<std::string, std::vector<size_t>> arrayNameToDimSizes;
};
