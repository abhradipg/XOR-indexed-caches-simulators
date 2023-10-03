#include "CacheLine.hpp"

#include <utility>

CacheLine::CacheLine(const Address &lastAccessedAddress)
    : lastAccessedAddress(lastAccessedAddress) {}

size_t CacheLine::GetMemoryBlockId() const {
  return this->lastAccessedAddress.GetMemoryBlockId();
}

const Address &CacheLine::GetLastAccessedAddress() const {
  return this->lastAccessedAddress;
}

size_t CacheLine::ComputeSymbolicHash(
    const GlobalIteratorState &globalIteratorState) const {
  const auto ca = this->lastAccessedAddress.GetConcreteAddress();
  const auto &sa = this->lastAccessedAddress.GetSymbolicAddress();
  return sa->ComputeHash(ca, globalIteratorState);
}

long CacheLine::GetCoefficient(size_t itId) const {
  return this->lastAccessedAddress.GetSymbolicAddress()->GetCoefficient(itId);
}

bool CacheLine::IsDirty() const { return this->dirty; }

void CacheLine::Access(const Address &address) {
  this->lastAccessedAddress = address;
}

void CacheLine::AccessAndMarkDirty(const Address &address) {
  this->lastAccessedAddress = address;
  this->dirty = true;
}

void CacheLine::AddToConcreteAddress(long amount) {
  this->lastAccessedAddress.AddToConcreteAddress(amount);
}

void CacheLine::Warp(const WarpInfo &warpInfo) {
  this->AddToConcreteAddress(this->GetCoefficient(warpInfo.GetItId()) *
                             warpInfo.GetItValDiff());
}
