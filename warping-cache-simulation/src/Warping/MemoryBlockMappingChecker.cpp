#include "MemoryBlockMappingChecker.hpp"

MemoryBlockMappingChecker::MemoryBlockMappingChecker(
    const SimulationAccessInfo &sai, const IteratorStateMap &ism)
    : iteratorStateMap(ism), sai(sai),
      mbRel(this->sai.GetMemoryBlockRelation()),
      mbRelDomainIdentity(IslUtil::GetBasicSetMultiAffIdentity(
          IslUtil::GetBasicMapDomain(this->mbRel))),
      mbRelRangeIdentity(IslUtil::GetBasicSetMultiAffIdentity(
          IslUtil::GetBasicMapRange(this->mbRel))) {}

bool MemoryBlockMappingChecker::IsOkay(size_t left, size_t right) const {
  assert(this->mbDelta && this->restrictedMbDomain);
  if (this->IsInRestrictedMbDomain(left) ||
      this->IsInRestrictedMbRange(right)) {
    const auto delta = (long)right - (long)left;
    return delta == this->mbDelta.value();
  }
  return true;
}

bool MemoryBlockMappingChecker::IsInAccessedArrayMbInterval(size_t mb) const {
  return Util::IsInInterval(mb,
                            this->sai.GetAccessedArrayMemoryBlockInterval());
}

bool MemoryBlockMappingChecker::IsInRestrictedMbDomain(size_t mb) const {
  if (!this->IsInAccessedArrayMbInterval(mb))
    return false;

  const long domMb = mb;
  const auto testDom = IslUtil::AddBasicSetEqConstraint(
      this->restrictedMbDomain.value(), this->mbRelRangeIdentity, 0, domMb);
  return !testDom.is_empty();
}

bool MemoryBlockMappingChecker::IsInRestrictedMbRange(size_t mb) const {
  if (!this->IsInAccessedArrayMbInterval(mb))
    return false;

  const long rangeMb = mb;
  const long domMb = rangeMb - this->mbDelta.value();
  if (domMb < 0)
    return false;

  const auto testDom = IslUtil::AddBasicSetEqConstraint(
      this->restrictedMbDomain.value(), this->mbRelRangeIdentity, 0, domMb);
  return !testDom.is_empty();
}

void MemoryBlockMappingChecker::Refresh(long furthestItVal,
                                        const SnapshotMatch &sm) {
  this->RefreshMbDelta(sm);
  this->RefreshRestrictedMbDomain(furthestItVal, sm);
}

void MemoryBlockMappingChecker::RefreshMbDelta(const SnapshotMatch &sm) {
  this->mbDelta.reset();
  this->mbDelta.emplace(this->ComputeMbDelta(sm));
}

void MemoryBlockMappingChecker::RefreshRestrictedMbDomain(
    long furthestItVal, const SnapshotMatch &sm) {
  this->restrictedMbDomain.reset();
  this->restrictedMbDomain.emplace(
      this->ComputeRestrictedMbDomain(furthestItVal, sm));
}

long MemoryBlockMappingChecker::ComputeMbDelta(const SnapshotMatch &sm) const {
  const auto itId = sm.GetIteratorId();
  const auto mappingItDistance = sm.GetNIterationsBetween();
  const auto mbPeriod = this->sai.GetDifferentMemoryBlockPeriod(itId);
  const auto mbStride = this->sai.GetDifferentMemoryBlockStride(itId);
  assert(mappingItDistance % mbPeriod == 0);
  return (long)(mappingItDistance / mbPeriod) * mbStride;
}

isl::basic_set MemoryBlockMappingChecker::ComputeRestrictedMbDomain(
    long furthestItVal, const SnapshotMatch &sm) const {
  const auto &restrictedMbRel = this->ComputeRestrictedMbRel(furthestItVal, sm);
  return IslUtil::GetBasicMapRange(restrictedMbRel);
}

isl::basic_map MemoryBlockMappingChecker::ComputeRestrictedMbRel(
    long furthestItVal, const SnapshotMatch &sm) const {
  auto restricted = this->mbRel;
  restricted = this->RestrictRelWithOuterIts(restricted);
  restricted = this->RestrictRelWithChangingIt(restricted, furthestItVal, sm);
  return restricted;
}

isl::basic_map MemoryBlockMappingChecker::RestrictRelWithOuterIts(
    const isl::basic_map &rel) const {
  auto restricted = rel;
  const auto changingItId = this->iteratorStateMap.GetChangingIteratorId();
  for (const auto &el : this->iteratorStateMap.GetIteratorStates()) {
    const auto itId = el.first;
    if (itId != changingItId) {
      assert(this->iteratorStateMap.IsOuterIteratorOfChangingIterator(itId));
      const auto &itUniqueName = el.second->GetIteratorUniqueName();
      const auto itVal = el.second->GetCurrentValue();
      restricted = IslUtil::AddBasicMapDomainEqConstraint(
          restricted, this->mbRelDomainIdentity, itUniqueName, itVal);
    }
  }
  return restricted;
}

isl::basic_map MemoryBlockMappingChecker::RestrictRelWithChangingIt(
    const isl::basic_map &rel, long furthestItVal,
    const SnapshotMatch &sm) const {
  auto restricted = rel;
  const auto &changingItUniqueName =
      this->iteratorStateMap.GetChangingIteratorUniqueName();
  const auto geVal = sm.GetOlderItVal();
  const auto ltVal = furthestItVal;
  restricted = IslUtil::AddBasicMapDomainGreaterEqConstraint(
      restricted, this->mbRelDomainIdentity, changingItUniqueName, geVal);
  restricted = IslUtil::AddBasicMapDomainLessConstraint(
      restricted, this->mbRelDomainIdentity, changingItUniqueName, ltVal);
  return restricted;
}
