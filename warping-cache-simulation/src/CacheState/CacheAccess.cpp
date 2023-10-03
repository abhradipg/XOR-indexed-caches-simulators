#include "CacheAccess.hpp"

CacheAccess::CacheAccess(bool isRead, const Address &address)
    : isRead(isRead), address(address) {}

const Address &CacheAccess::GetAddress() const { return this->address; }

size_t CacheAccess::GetMemoryBlockId() const {
  return this->address.GetMemoryBlockId();
}

size_t CacheAccess::GetConcreteAddress() const {
  return this->address.GetConcreteAddress();
}

const SymbolicAddress *CacheAccess::GetSymbolicAddress() const {
  return this->address.GetSymbolicAddress();
}

bool CacheAccess::IsRead() const { return this->isRead; }

std::ostream &operator<<(std::ostream &os, const CacheAccess &a) {
  os << (a.IsRead() ? "(R) " : "(W) ") << a.GetConcreteAddress();
  return os;
}
