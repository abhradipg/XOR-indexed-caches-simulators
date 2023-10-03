#include "Address.hpp"

Address::Address(size_t concreteAddress, const SymbolicAddress *symbolicAddress)
    : concreteAddress(concreteAddress), symbolicAddress(symbolicAddress) {
  assert(this->symbolicAddress != nullptr);
}

size_t Address::GetConcreteAddress() const { return this->concreteAddress; }

const SymbolicAddress *Address::GetSymbolicAddress() const {
  return this->symbolicAddress;
}

size_t Address::GetMemoryBlockId() const {
  return Util::ComputeMemoryBlock(this->concreteAddress);
}

void Address::AddToConcreteAddress(long amount) {
  const auto newConcreteAddress = (long)this->concreteAddress + amount;
  assert(newConcreteAddress >= 0);
  this->concreteAddress = newConcreteAddress;
}
