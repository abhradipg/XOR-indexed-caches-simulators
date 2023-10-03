#pragma once

#include <isl/cpp.h>
#include <utility>
#include <vector>

#include "Address/SymbolicAddress.hpp"
#include "CacheState/CacheAccess.hpp"
#include "ProgramInfo/ArrayInfo.hpp"
#include "Simulation/SimulationConditionInfo.hpp"

class AccessInfo {
public:
  AccessInfo(bool isRead, const ArrayInfo &arrayInfo,
             const isl::map &taggedReadOrWrite);

  void RenameSymbolicVars(
      const std::unordered_map<std::string, std::string> &newNames);

  [[nodiscard]] isl::ctx GetIslCtx() const;

  [[nodiscard]] const isl::set &GetDomain() const;

  [[nodiscard]] const isl::aff &GetAffineAddress() const;

  [[nodiscard]] std::pair<size_t, size_t>
  GetAccessedArrayMemoryBlockInterval() const;

  [[nodiscard]] bool IsRead() const;

  bool operator<(const AccessInfo &other) const;

private:
  bool isRead;
  std::string refId;
  ArrayInfo arrayInfo;
  isl::aff affineAddress;
  isl::set domain;
};
