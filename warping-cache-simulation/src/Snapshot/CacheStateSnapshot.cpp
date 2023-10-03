#include "CacheStateSnapshot.hpp"

CacheStateSnapshot::CacheStateSnapshot(size_t nLines, size_t nCacheSets,
                                       size_t itId)
    : changingItId(itId), nCacheSets(nCacheSets),
      symbolicHash(std::hash<size_t>()(nLines)) {
  this->lines.reserve(nLines);
}

void CacheStateSnapshot::AddLine(
    const CacheLine &line, const GlobalIteratorState &globalIteratorState) {
  this->lines.push_back(line);
  this->ComputeAndCombineLineHash(line, globalIteratorState);
  if (this->lines.size() == 1)
    this->InitCoefficientInfo(line);
  else
    this->UpdateCoefficientInfo(line);
}

void CacheStateSnapshot::SetLastAccessedSetIndex(size_t index) {
  this->lastAccessedSetIndex = index;
}

void CacheStateSnapshot::ComputeAndCombineLineHash(
    const CacheLine &line, const GlobalIteratorState &globalIteratorState) {
  const auto lineHash = line.ComputeSymbolicHash(globalIteratorState);
  Util::HashCombine(lineHash, this->symbolicHash);
}

void CacheStateSnapshot::InitCoefficientInfo(const CacheLine &line) {
  this->hasSingleCoefficient = true;
  this->singleCoefficient.emplace(line.GetCoefficient(this->changingItId));
}

void CacheStateSnapshot::UpdateCoefficientInfo(const CacheLine &line) {
  if (this->hasSingleCoefficient) {
    assert(this->singleCoefficient);
    if (line.GetCoefficient(this->changingItId) != *this->singleCoefficient) {
      this->hasSingleCoefficient = false;
      this->singleCoefficient.reset();
    }
  }
}

const std::vector<CacheLine> &CacheStateSnapshot::GetLines() const {
  return this->lines;
}

bool CacheStateSnapshot::DoesHaveSingleCoefficient() const {
  return this->hasSingleCoefficient;
}

long CacheStateSnapshot::GetSingleCoefficient() const {
  assert(this->singleCoefficient);
  return *this->singleCoefficient;
}

size_t CacheStateSnapshot::GetSymbolicHash() const {
  return this->symbolicHash;
}

size_t CacheStateSnapshot::GetLastAccessedSetIndex() const {
  return this->lastAccessedSetIndex;
}

bool CacheStateSnapshot::IsSymbolicallyEqual(
    const CacheStateSnapshot &other) const {
  assert(this->symbolicHash == other.symbolicHash);
  assert(this->lines.size() == other.lines.size());
  for (size_t i = 0; i < lines.size(); i++) {
    const auto &sa1 =
        this->lines[i].GetLastAccessedAddress().GetSymbolicAddress();
    const auto &sa2 =
        other.lines[i].GetLastAccessedAddress().GetSymbolicAddress();
    if (sa1->GetItIdsToCoefficients() != sa2->GetItIdsToCoefficients())
      return false;
    // TODO: consider also the shifted constants...
  }
  return true;
}

size_t CacheStateSnapshot::GetConcreteAddress(size_t lineIndex) const {
  return this->lines[lineIndex].GetLastAccessedAddress().GetConcreteAddress();
}

void CacheStateSnapshot::WarpCacheLine(size_t lineIndex,
                                       long concreteAddressDiff) {
  this->lines[lineIndex].AddToConcreteAddress(concreteAddressDiff);
}

void CacheStateSnapshot::Rotate(size_t nRightRotations) {
  assert(this->nCacheSets > 1);
  const auto n = nRightRotations % this->nCacheSets;
  this->lastAccessedSetIndex =
      (this->lastAccessedSetIndex + n) % this->nCacheSets;
}
