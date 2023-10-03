#pragma once

#include <cassert>
#include <string>

class IteratorInfo {
public:
  IteratorInfo(size_t id, std::string name, std::string uniqueName,
               long stride);

  [[nodiscard]] size_t GetId() const;

  [[nodiscard]] const std::string &GetName() const;

  [[nodiscard]] const std::string &GetUniqueName() const;

  [[nodiscard]] long GetStride() const;

private:
  const size_t id;
  const std::string name;
  const std::string uniqueName;
  const long stride;
};
