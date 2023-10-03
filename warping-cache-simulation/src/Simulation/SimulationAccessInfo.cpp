#include "SimulationAccessInfo.hpp"

SimulationAccessInfo::SimulationAccessInfo(
    const AccessInfo &accessInfo, const isl::set &parentIterationDomain,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap), isRead(accessInfo.IsRead()),
      accessedArrayMemoryBlockInterval(
          accessInfo.GetAccessedArrayMemoryBlockInterval()),
      accessDomain(IslUtil::RenameSetDims(
          accessInfo.GetDomain(),
          iteratorStateMap.GetIteratorNamesToIteratorUniqueNames())),
      accessDomainIteratorUniqueNames(
          this->ExtractAccessDomainIteratorUniqueNames()),
      accessDomainIteratorIds(this->ExtractAccessDomainIteratorIds()),
      affineAddress(IslUtil::RenameAffDims(
          accessInfo.GetAffineAddress(),
          iteratorStateMap.GetIteratorNamesToIteratorUniqueNames())),
      symbolicAddress(new SymbolicAddress(
          this->affineAddress,
          iteratorStateMap.GetIteratorUniqueNamesToIteratorIds())),
      mbRel(this->ComputeMbRel()),
      simulationConditionInfo(this->accessDomain, parentIterationDomain,
                              iteratorStateMap),
      addrDeltaPerItByItId(this->ComputeAddrDeltaPerItByItId()),
      diffMbPeriodByItId(this->ComputeDiffMbPeriodByItId()),
      diffMbStrideByItId(this->ComputeDiffMbStrideByItId()) {}

SimulationAccessInfo::~SimulationAccessInfo() { delete this->symbolicAddress; }

const SymbolicAddress *SimulationAccessInfo::GetSymbolicAddress() const {
  return this->symbolicAddress;
}

Address SimulationAccessInfo::GetCurrentAddress() const {
  return {this->GetCurrentConcreteAddress(), this->GetSymbolicAddress()};
}

size_t SimulationAccessInfo::GetCurrentConcreteAddress() const {
  return this->symbolicAddress->Eval(
      this->iteratorStateMap.GetCurrentIteratorValuePointers());
}

const std::pair<size_t, size_t> &
SimulationAccessInfo::GetAccessedArrayMemoryBlockInterval() const {
  return this->accessedArrayMemoryBlockInterval;
}

const isl::aff &SimulationAccessInfo::GetAffineAddress() const {
  return this->affineAddress;
}

const isl::set &SimulationAccessInfo::GetAccessDomain() const {
  return this->accessDomain;
}

const isl::basic_map &SimulationAccessInfo::GetMemoryBlockRelation() const {
  return this->mbRel;
}

const std::set<size_t> &
SimulationAccessInfo::GetAccessDomainIteratorIds() const {
  return this->accessDomainIteratorIds;
}

const std::set<std::string> &
SimulationAccessInfo::GetAccessDomainIteratorUniqueNames() const {
  return this->accessDomainIteratorUniqueNames;
}

const SimulationConditionInfo &
SimulationAccessInfo::GetSimulationConditionInfo() const {
  return this->simulationConditionInfo;
}

bool SimulationAccessInfo::IsRead() const { return this->isRead; }

bool SimulationAccessInfo::IsConditional() const {
  return this->simulationConditionInfo.IsConditional();
}

bool SimulationAccessInfo::ShouldSimulate() const {
  return !this->IsConditional() ||
         this->simulationConditionInfo.DoesConditionHold();
}

bool SimulationAccessInfo::IsAccessedArrayMemoryBlockIntervalDisjoint(
    const SimulationAccessInfo &other) const {
  return Util::AreIntervalsDisjoint(this->accessedArrayMemoryBlockInterval,
                                    other.accessedArrayMemoryBlockInterval);
}

long SimulationAccessInfo::GetAddressDeltaPerIteration(size_t itId) const {
  const auto it = this->addrDeltaPerItByItId.find(itId);
  assert(it != this->addrDeltaPerItByItId.end());
  return it->second;
}

size_t SimulationAccessInfo::GetDifferentMemoryBlockPeriod(size_t itId) const {
  const auto it = this->diffMbPeriodByItId.find(itId);
  assert(it != this->diffMbPeriodByItId.end());
  return it->second;
}

long SimulationAccessInfo::GetDifferentMemoryBlockStride(size_t itId) const {
  const auto it = this->diffMbStrideByItId.find(itId);
  assert(it != this->diffMbStrideByItId.end());
  return it->second;
}

std::set<std::string>
SimulationAccessInfo::ExtractAccessDomainIteratorUniqueNames() const {
  const auto &dimStrs = IslUtil::ConvertBasicSetTupleToStringVector(
      this->accessDomain.polyhedral_hull());
  std::set<std::string> itUniqueNames;
  for (const auto &itUniqueName : dimStrs) {
    assert(itUniqueNames.count(itUniqueName) == 0);
    itUniqueNames.insert(itUniqueName);
  }
  return itUniqueNames;
}

std::set<size_t> SimulationAccessInfo::ExtractAccessDomainIteratorIds() const {
  std::set<size_t> itIds;
  for (const auto &itUniqueName : this->accessDomainIteratorUniqueNames) {
    const auto itId = this->iteratorStateMap.GetIteratorId(itUniqueName);
    assert(itIds.count(itId) == 0);
    itIds.insert(itId);
  }
  return itIds;
}

std::unordered_map<size_t, long>
SimulationAccessInfo::ComputeAddrDeltaPerItByItId() const {
  std::unordered_map<size_t, long> res;
  for (const auto &el : this->iteratorStateMap.GetIteratorStates()) {
    const auto itId = el.second->GetIteratorId();
    const auto coefficient = this->symbolicAddress->GetCoefficient(itId);
    const auto stride = el.second->GetStride();
    const long delta = coefficient * stride;
    assert(res.count(itId) == 0);
    res.emplace(itId, delta);
  }
  return res;
}

std::unordered_map<size_t, size_t>
SimulationAccessInfo::ComputeDiffMbPeriodByItId() const {
  std::unordered_map<size_t, size_t> res;
  for (const auto &el : this->addrDeltaPerItByItId) {
    const auto itId = el.first;
    const auto absDelta = std::abs(el.second);
    const auto period =
        GlobalVars::lineSize / std::gcd(GlobalVars::lineSize, absDelta);
    assert(res.count(itId) == 0);
    res.emplace(itId, period);
  }
  return res;
}

std::unordered_map<size_t, long>
SimulationAccessInfo::ComputeDiffMbStrideByItId() const {
  std::unordered_map<size_t, long> res;
  for (const auto &el : this->diffMbPeriodByItId) {
    const auto itId = el.first;
    const auto period = (long)el.second;
    const auto delta = this->GetAddressDeltaPerIteration(itId);
    const auto mbStride = Util::ComputeMemoryBlockDelta(delta * period);
    assert(res.count(itId) == 0);
    res.emplace(itId, mbStride);
  }
  return res;
}

isl::basic_map SimulationAccessInfo::ComputeMbRel() const {
  auto pwAff =
      IslUtil::ConvertAffToPwAff(this->affineAddress, this->accessDomain);
  auto bMap = IslUtil::ConvertPwAffToBasicMap(pwAff);
  return IslUtil::DivAndFloorBasicMap(bMap, GlobalVars::lineSize);
}
