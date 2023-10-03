#pragma once

#include <unordered_map>

#include "Address/Address.hpp"
#include "IteratorState/GlobalIteratorState.hpp"
#include "Util/Util.hpp"
#include "Warping/WarpInfo.hpp"

class CacheLine {
public:
  explicit CacheLine(const Address &address);

  [[nodiscard]] size_t GetMemoryBlockId() const;

  [[nodiscard]] const Address &GetLastAccessedAddress() const;

  [[nodiscard]] size_t
  ComputeSymbolicHash(const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] long GetCoefficient(size_t itId) const;

  [[nodiscard]] bool IsDirty() const;

  void Access(const Address &address);

  void AccessAndMarkDirty(const Address &address);

  void AddToConcreteAddress(long amount);

  void Warp(const WarpInfo &warpInfo);

private:
  Address lastAccessedAddress;

  bool dirty = false;
};
