#pragma once

#include <numeric>

#include "Address/SymbolicAddress.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "ProgramInfo/AccessInfo.hpp"
#include "Simulation/SimulationConditionInfo.hpp"
#include "Util/IslUtil.hpp"

class SimulationAccessInfo {
public:
  SimulationAccessInfo(const AccessInfo &accessInfo,
                       const isl::set &parentIterationDomain,
                       const IteratorStateMap &iteratorStateMap);

  ~SimulationAccessInfo();

  [[nodiscard]] const SymbolicAddress *GetSymbolicAddress() const;

  [[nodiscard]] Address GetCurrentAddress() const;

  [[nodiscard]] size_t GetCurrentConcreteAddress() const;

  [[nodiscard]] const std::pair<size_t, size_t> &
  GetAccessedArrayMemoryBlockInterval() const;

  [[nodiscard]] const isl::aff &GetAffineAddress() const;

  [[nodiscard]] const isl::set &GetAccessDomain() const;

  [[nodiscard]] const isl::basic_map &GetMemoryBlockRelation() const;

  [[nodiscard]] const std::set<size_t> &GetAccessDomainIteratorIds() const;

  [[nodiscard]] const std::set<std::string> &
  GetAccessDomainIteratorUniqueNames() const;

  [[nodiscard]] const SimulationConditionInfo &
  GetSimulationConditionInfo() const;

  [[nodiscard]] bool IsRead() const;

  [[nodiscard]] bool IsConditional() const;

  [[nodiscard]] bool ShouldSimulate() const;

  [[nodiscard]] bool IsAccessedArrayMemoryBlockIntervalDisjoint(
      const SimulationAccessInfo &other) const;

  [[nodiscard]] long GetAddressDeltaPerIteration(size_t itId) const;

  [[nodiscard]] size_t GetDifferentMemoryBlockPeriod(size_t itId) const;

  [[nodiscard]] long GetDifferentMemoryBlockStride(size_t itId) const;

private:
  const IteratorStateMap &iteratorStateMap;

  const bool isRead;

  const std::pair<size_t, size_t> accessedArrayMemoryBlockInterval;

  const isl::set accessDomain;
  const std::set<std::string> accessDomainIteratorUniqueNames;
  const std::set<size_t> accessDomainIteratorIds;

  const isl::aff affineAddress;
  const SymbolicAddress *const symbolicAddress;

  const isl::basic_map mbRel;

  const SimulationConditionInfo simulationConditionInfo;

  const std::unordered_map<size_t, long> addrDeltaPerItByItId;
  const std::unordered_map<size_t, size_t> diffMbPeriodByItId;
  const std::unordered_map<size_t, long> diffMbStrideByItId;

  [[nodiscard]] std::set<std::string>
  ExtractAccessDomainIteratorUniqueNames() const;

  [[nodiscard]] std::set<size_t> ExtractAccessDomainIteratorIds() const;

  [[nodiscard]] std::unordered_map<size_t, long>
  ComputeAddrDeltaPerItByItId() const;

  [[nodiscard]] std::unordered_map<size_t, size_t>
  ComputeDiffMbPeriodByItId() const;

  [[nodiscard]] std::unordered_map<size_t, long>
  ComputeDiffMbStrideByItId() const;

  [[nodiscard]] isl::basic_map ComputeMbRel() const;
};
