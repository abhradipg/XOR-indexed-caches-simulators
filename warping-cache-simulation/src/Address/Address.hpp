#pragma once

#include <utility>

#include "Address/SymbolicAddress.hpp"
#include "Util/Util.hpp"

class Address {
public:
  Address(size_t concreteAddress, const SymbolicAddress *symbolicAddress);

  [[nodiscard]] size_t GetConcreteAddress() const;

  [[nodiscard]] const SymbolicAddress *GetSymbolicAddress() const;

  [[nodiscard]] size_t GetMemoryBlockId() const;

  void AddToConcreteAddress(long amount);

private:
  size_t concreteAddress;
  const SymbolicAddress *symbolicAddress;
};
