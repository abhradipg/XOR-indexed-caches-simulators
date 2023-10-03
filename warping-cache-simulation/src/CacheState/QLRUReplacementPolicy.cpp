#include "QLRUReplacementPolicy.hpp"

QLRUReplacementPolicy::QLRUReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState,
    const QLRUSettings &qlruSettings)
    : ReplacementPolicy(nLines, globalIteratorState),
      hitFunction(this->CreateHitFunction(qlruSettings)),
      replaceFunction(this->CreateReplaceFunction(qlruSettings)),
      updateFunction(this->CreateUpdateFunction(qlruSettings)),
      ageAfterMiss(qlruSettings.GetInsertionAge()) {
  this->lineIndicesByAge.resize(4);
  this->lineInfos.resize(this->nLines);
  this->leftMostEmptyLineIndex.emplace(0);
  this->rightMostEmptyLineIndex.emplace(this->nLines - 1);
}

size_t QLRUReplacementPolicy::GetNumberOfNonEmptyLines() const {
  return this->memoryBlockToLineIndex.size();
}

bool QLRUReplacementPolicy::DoesContainMemoryBlock(size_t memoryBlock) const {
  return this->memoryBlockToLineIndex.count(memoryBlock) == 1;
}

std::optional<CacheLine>
QLRUReplacementPolicy::AddNewCacheLineAndGetEvicted(const Address &address) {
  std::optional<CacheLine> evicted;
  const auto lineIndex = this->replaceFunction();
  if (this->IsLineEmpty(lineIndex))
    this->FillEmptyLineIndex(lineIndex, address);
  else {
    evicted.emplace(this->GetLineInfoRef(lineIndex).first);
    this->AccessNonEmptyLineIndexOnMiss(lineIndex, address);
  }
  this->updateFunction(lineIndex);
  return evicted;
}

void QLRUReplacementPolicy::AccessToExistingCacheLine(const Address &address) {
  const auto lineIndex = this->GetNonEmptyLineIndex(address);
  this->AccessNonEmptyLineIndexOnHit(lineIndex, address);
  this->updateFunction(lineIndex);
}

void QLRUReplacementPolicy::AccessToExistingCacheLineAndMarkDirty(
    const Address &address) {
  const auto lineIndex = this->GetNonEmptyLineIndex(address);
  this->AccessNonEmptyLineIndexAndMarkDirtyOnHit(lineIndex, address);
  this->updateFunction(lineIndex);
}

void QLRUReplacementPolicy::AddLinesToCacheStateSnapshot(
    CacheStateSnapshot &cacheStateSnapshot) const {
  // this->AddLinesToCacheStateSnapshotInIndexOrder(cacheStateSnapshot);
  this->AddLinesToCacheStateSnapshotInAgeOrder(cacheStateSnapshot);
}

void QLRUReplacementPolicy::AddLinesToCacheStateSnapshotInAgeOrder(
    CacheStateSnapshot &cacheStateSnapshot) const {
  for (Age age = 0; age < 3; age++)
    for (const auto i : this->lineIndicesByAge[age])
      cacheStateSnapshot.AddLine(this->lineInfos[i]->first,
                                 this->globalIteratorState);
}

void QLRUReplacementPolicy::AddLinesToCacheStateSnapshotInIndexOrder(
    CacheStateSnapshot &cacheStateSnapshot) const {
  for (const auto &optLineInfo : this->lineInfos) {
    if (optLineInfo) {
      auto &cacheLine = optLineInfo.value().first;
      cacheStateSnapshot.AddLine(cacheLine, this->globalIteratorState);
    }
  }
}

void QLRUReplacementPolicy::Warp(const WarpInfo &warpInfo) {
  this->memoryBlockToLineIndex.clear();
  for (size_t lineIndex = 0; lineIndex < this->lineInfos.size(); lineIndex++) {
    auto &optLineInfo = this->lineInfos[lineIndex];
    if (optLineInfo) {
      auto &cacheLine = optLineInfo.value().first;
      cacheLine.Warp(warpInfo);
      const auto newMb = cacheLine.GetMemoryBlockId();
      assert(this->memoryBlockToLineIndex.count(newMb) == 0);
      this->memoryBlockToLineIndex.emplace(newMb, lineIndex);
    }
  }
}

bool QLRUReplacementPolicy::IsLineEmpty(size_t lineIndex) const {
  return !this->lineInfos[lineIndex];
}

QLRUReplacementPolicy::Age
QLRUReplacementPolicy::GetCurrentAge(size_t lineIndex) const {
  for (Age age = 0; age < 4; age++)
    if (this->lineIndicesByAge[age].count(lineIndex))
      return age;
  assert(false);
  return 0;
}

QLRUReplacementPolicy::LineInfo &
QLRUReplacementPolicy::GetLineInfoRef(size_t lineIndex) {
  auto &optLineInfo = this->GetOptLineInfoRef(lineIndex);
  assert(optLineInfo);
  return optLineInfo.value();
}

std::optional<QLRUReplacementPolicy::LineInfo> &
QLRUReplacementPolicy::GetOptLineInfoRef(size_t lineIndex) {
  return this->lineInfos[lineIndex];
}

size_t QLRUReplacementPolicy::GetMemoryBlockAtLineIndex(size_t lineIndex) {
  return this->GetLineInfoRef(lineIndex).first.GetMemoryBlockId();
}

void QLRUReplacementPolicy::ChangeMemoryBlockToLineIndexKey(size_t oldKey,
                                                            size_t newKey) {
  auto nh = this->memoryBlockToLineIndex.extract(oldKey);
  assert(!nh.empty());
  nh.key() = newKey;
  this->memoryBlockToLineIndex.insert(move(nh));
}

void QLRUReplacementPolicy::InsertToMemoryBlockToLineIndex(size_t memoryBlock,
                                                           size_t lineIndex) {
  auto res = this->memoryBlockToLineIndex.emplace(memoryBlock, lineIndex);
  assert(res.second);
}

void QLRUReplacementPolicy::AccessNonEmptyLineIndexOnMiss(
    size_t lineIndex, const Address &address) {
  const auto currentMb = this->GetMemoryBlockAtLineIndex(lineIndex);
  const auto newMb = address.GetMemoryBlockId();
  this->ChangeMemoryBlockToLineIndexKey(currentMb, newMb);

  const auto currentAge = this->GetCurrentAge(lineIndex);
  const auto newAge = this->ageAfterMiss;
  this->AccessNonEmptyLineIndexAndChangeItsAge(lineIndex, address, currentAge,
                                               newAge);
}

void QLRUReplacementPolicy::AccessNonEmptyLineIndexOnHit(
    size_t lineIndex, const Address &address) {
  const auto currentAge = this->GetCurrentAge(lineIndex);
  const auto newAge = this->hitFunction(currentAge);
  this->AccessNonEmptyLineIndexAndChangeItsAge(lineIndex, address, currentAge,
                                               newAge);
}

void QLRUReplacementPolicy::AccessNonEmptyLineIndexAndMarkDirtyOnHit(
    size_t lineIndex, const Address &address) {
  const auto currentAge = this->GetCurrentAge(lineIndex);
  const auto newAge = this->hitFunction(currentAge);
  this->AccessNonEmptyLineIndexAndMarkDirtyAndChangeItsAge(lineIndex, address,
                                                           currentAge, newAge);
}

void QLRUReplacementPolicy::AccessNonEmptyLineIndexAndChangeItsAge(
    size_t lineIndex, const Address &address, Age currentAge, Age newAge) {
  auto &lineInfo = this->GetLineInfoRef(lineIndex);
  auto &cacheLine = lineInfo.first;
  auto &it = lineInfo.second;
  cacheLine.Access(address);
  this->EraseAgeForLineIndex(currentAge, it);
  it = this->InsertAgeForLineIndex(lineIndex, newAge);
}

void QLRUReplacementPolicy::AccessNonEmptyLineIndexAndMarkDirtyAndChangeItsAge(
    size_t lineIndex, const Address &address,
    QLRUReplacementPolicy::Age currentAge, QLRUReplacementPolicy::Age newAge) {
  auto &lineInfo = this->GetLineInfoRef(lineIndex);
  auto &cacheLine = lineInfo.first;
  auto &it = lineInfo.second;
  cacheLine.AccessAndMarkDirty(address);
  this->EraseAgeForLineIndex(currentAge, it);
  it = this->InsertAgeForLineIndex(lineIndex, newAge);
}

void QLRUReplacementPolicy::EraseAgeForLineIndex(
    Age age, const std::set<size_t>::iterator &it) {
  this->lineIndicesByAge[age].erase(it);
}

void QLRUReplacementPolicy::ShiftAllAges(Age ageShift) {
  assert(ageShift != 0);
  auto &age2target = this->lineIndicesByAge[3];
  auto &age1target = this->lineIndicesByAge[std::min(1 + ageShift, 3)];
  auto &age0target = this->lineIndicesByAge[std::min(0 + ageShift, 3)];

  age2target.merge(this->lineIndicesByAge[2]);
  assert(this->lineIndicesByAge[2].empty());
  age1target.merge(this->lineIndicesByAge[1]);
  assert(this->lineIndicesByAge[1].empty());
  age0target.merge(this->lineIndicesByAge[0]);
  assert(this->lineIndicesByAge[0].empty());
}

void QLRUReplacementPolicy::ShiftAgesExceptLine(Age ageShift,
                                                size_t lineIndex) {
  auto &lineInfo = this->GetLineInfoRef(lineIndex);
  const auto currentAge = this->GetCurrentAge(lineIndex);
  auto &lineIndexSetIt = lineInfo.second;
  this->EraseAgeForLineIndex(currentAge, lineIndexSetIt);
  this->ShiftAllAges(ageShift);
  lineIndexSetIt = this->InsertAgeForLineIndex(lineIndex, currentAge);
}

QLRUReplacementPolicy::LineIndexSet::iterator
QLRUReplacementPolicy::InsertAgeForLineIndex(size_t lineIndex, Age age) {
  const auto &insertRes = this->lineIndicesByAge[age].insert(lineIndex);
  assert(insertRes.second);
  return insertRes.first;
}

void QLRUReplacementPolicy::AccessLeftMostEmptyLineIndex(
    const Address &address) {
  assert(this->leftMostEmptyLineIndex);
  this->FillEmptyLineIndex(this->leftMostEmptyLineIndex.value(), address);
}

void QLRUReplacementPolicy::AccessRightMostEmptyLineIndex(
    const Address &address) {
  assert(this->rightMostEmptyLineIndex);
  this->FillEmptyLineIndex(this->rightMostEmptyLineIndex.value(), address);
}

void QLRUReplacementPolicy::FillEmptyLineIndex(size_t lineIndex,
                                               const Address &address) {
  this->InsertToMemoryBlockToLineIndex(address.GetMemoryBlockId(), lineIndex);

  auto &optLineInfo = this->GetOptLineInfoRef(lineIndex);
  assert(!optLineInfo);
  auto it = this->InsertAgeForLineIndex(lineIndex, this->ageAfterMiss);
  optLineInfo = std::make_optional<LineInfo>(address, it);

  if (this->leftMostEmptyLineIndex &&
      this->leftMostEmptyLineIndex.value() == lineIndex)
    this->RefreshLeftMostEmptyLineIndex();
  else if (this->rightMostEmptyLineIndex &&
           this->rightMostEmptyLineIndex.value() == lineIndex)
    this->RefreshRightMostEmptyLineIndex();
}

void QLRUReplacementPolicy::RefreshLeftMostEmptyLineIndex() {
  assert(this->leftMostEmptyLineIndex);
  const auto leftMost = this->leftMostEmptyLineIndex.value();
  this->leftMostEmptyLineIndex.reset();
  if (!this->rightMostEmptyLineIndex)
    return;
  else if (this->rightMostEmptyLineIndex.value() == leftMost)
    this->rightMostEmptyLineIndex.reset();
  else {
    const auto rightMost = this->rightMostEmptyLineIndex.value();
    assert(leftMost < rightMost);
    for (size_t i = leftMost + 1; i <= rightMost; i++) {
      if (this->IsLineIndexEmpty(i)) {
        this->leftMostEmptyLineIndex.emplace(i);
        break;
      }
    }
    assert(this->leftMostEmptyLineIndex);
  }
}

void QLRUReplacementPolicy::RefreshRightMostEmptyLineIndex() {
  assert(this->rightMostEmptyLineIndex);
  const auto rightMost = this->rightMostEmptyLineIndex.value();
  this->rightMostEmptyLineIndex.reset();
  if (!this->leftMostEmptyLineIndex)
    return;
  else if (this->leftMostEmptyLineIndex.value() == rightMost)
    this->leftMostEmptyLineIndex.reset();
  else {
    const auto leftMost = this->leftMostEmptyLineIndex.value();
    assert(leftMost < rightMost && rightMost >= 1);
    for (size_t i = rightMost - 1; i > leftMost; i--) {
      if (this->IsLineIndexEmpty(i)) {
        this->rightMostEmptyLineIndex.emplace(i);
        break;
      }
    }
    if (!this->rightMostEmptyLineIndex)
      this->rightMostEmptyLineIndex.emplace(leftMost);
  }
}

bool QLRUReplacementPolicy::IsLineIndexEmpty(size_t lineIndex) const {
  return !this->lineInfos[lineIndex];
}

QLRUReplacementPolicy::Age QLRUReplacementPolicy::GetMaxAge() const {
  if (!this->lineIndicesByAge[3].empty())
    return 3;
  else if (!this->lineIndicesByAge[2].empty())
    return 2;
  else if (!this->lineIndicesByAge[1].empty())
    return 1;
  assert(!this->lineIndicesByAge[0].empty());
  return 0;
}

size_t
QLRUReplacementPolicy::GetNonEmptyLineIndex(const Address &address) const {
  auto it = this->memoryBlockToLineIndex.find(address.GetMemoryBlockId());
  assert(it != this->memoryBlockToLineIndex.end());
  return it->second;
}

std::function<QLRUReplacementPolicy::Age(QLRUReplacementPolicy::Age)>
QLRUReplacementPolicy::CreateHitFunction(
    const QLRUSettings &qlruSettings) const {
  if (qlruSettings.IsHitH00())
    return [](Age) { return 0; };
  else if (qlruSettings.IsHitH01())
    return [](Age age) { return age == 2 ? 1 : 0; };
  else if (qlruSettings.IsHitH10())
    return [](Age age) { return age == 3 ? 1 : 0; };
  else if (qlruSettings.IsHitH11())
    return [](Age age) { return age > 1 ? 1 : 0; };
  else if (qlruSettings.IsHitH20())
    return [](Age age) { return age == 3 ? 2 : 0; };
  assert(qlruSettings.IsHitH21());
  return [](Age age) { return age == 3 ? 2 : age == 2 ? 1 : 0; };
}

std::function<size_t()>
QLRUReplacementPolicy::CreateReplaceFunction(const QLRUSettings &qlruSettings) {
  if (qlruSettings.IsReplaceR0())
    return this->CreateReplaceFunctionR0();
  else if (qlruSettings.IsReplaceR1())
    return this->CreateReplaceFunctionR1();
  assert(qlruSettings.IsReplaceR2());
  return this->CreateReplaceFunctionR2();
}

std::function<void(size_t)>
QLRUReplacementPolicy::CreateUpdateFunction(const QLRUSettings &qlruSettings) {
  if (qlruSettings.IsUpdateU0())
    return this->CreateUpdateFunctionU0();
  else if (qlruSettings.IsUpdateU1())
    return this->CreateUpdateFunctionU1();
  else if (qlruSettings.IsUpdateU2())
    return this->CreateUpdateFunctionU2();
  assert(qlruSettings.IsUpdateU3());
  return this->CreateUpdateFunctionU3();
}

std::function<size_t()> QLRUReplacementPolicy::CreateReplaceFunctionR0() {
  return [&]() {
    if (this->IsFull()) {
      // Normally, it has undefined behaviour when there is no line with age 3!
      // Instead, we replace the left most line with current max age.
      return *(this->lineIndicesByAge[this->GetMaxAge()].begin());
    }
    return this->leftMostEmptyLineIndex.value();
  };
}

std::function<size_t()> QLRUReplacementPolicy::CreateReplaceFunctionR1() {
  return [&]() {
    if (this->IsFull()) {
      const auto &lineIndices = this->lineIndicesByAge[3];
      return lineIndices.empty() ? 0 : *lineIndices.begin();
    }
    return this->leftMostEmptyLineIndex.value();
  };
}

std::function<size_t()> QLRUReplacementPolicy::CreateReplaceFunctionR2() {
  return [&]() {
    if (this->IsFull())
      return *(this->lineIndicesByAge[this->GetMaxAge()].begin());
    return this->rightMostEmptyLineIndex.value();
  };
}

std::function<void(size_t)> QLRUReplacementPolicy::CreateUpdateFunctionU0() {
  return [&](size_t) {
    const auto maxAge = this->GetMaxAge();
    const auto ageShift = 3 - maxAge;
    if (ageShift == 0)
      return;
    this->ShiftAllAges(ageShift);
  };
}

std::function<void(size_t)> QLRUReplacementPolicy::CreateUpdateFunctionU1() {
  return [&](size_t lineIndex) {
    const auto maxAge = this->GetMaxAge();
    const auto ageShift = 3 - maxAge;
    if (ageShift == 0)
      return;
    this->ShiftAgesExceptLine(ageShift, lineIndex);
  };
}

std::function<void(size_t)> QLRUReplacementPolicy::CreateUpdateFunctionU2() {
  return [&](size_t) { this->ShiftAllAges(1); };
}

std::function<void(size_t)> QLRUReplacementPolicy::CreateUpdateFunctionU3() {
  return [&](size_t lineIndex) { this->ShiftAgesExceptLine(1, lineIndex); };
}
