#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Address/Address.hpp"

class CacheAccess {
public:
  CacheAccess(bool isRead, const Address &address);

  [[nodiscard]] const Address &GetAddress() const;

  [[nodiscard]] size_t GetMemoryBlockId() const;

  [[nodiscard]] size_t GetConcreteAddress() const;

  [[nodiscard]] const SymbolicAddress *GetSymbolicAddress() const;

  [[nodiscard]] bool IsRead() const;

private:
  const bool isRead;
  const Address address;

  friend std::ostream &operator<<(std::ostream &os, const CacheAccess &a);
};
