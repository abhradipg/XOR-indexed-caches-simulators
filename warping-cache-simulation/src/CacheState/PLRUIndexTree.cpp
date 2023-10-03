#include "PLRUIndexTree.hpp"

PLRUIndexTree::PLRUIndexTree(size_t nLines,
                             const GlobalIteratorState &globalIteratorState)
    : globalIteratorState(globalIteratorState) {
  assert(Util::IsPowerOfTwo(nLines));
  const auto nLeafIndices = nLines;
  const auto nBits = nLeafIndices - 1;
  this->bits.resize(nBits, false);
  this->isBranchNonEmpty.resize(nBits + nLeafIndices, false);
  this->bitsLeftLeafIndexIntervals.resize(this->bits.size());
  this->bitsRightLeafIndexIntervals.resize(this->bits.size());
  this->InitBitsLeafIndexIntervalsRecursively(0, {0, nLeafIndices - 1});
}

void PLRUIndexTree::InitBitsLeafIndexIntervalsRecursively(
    size_t bitIndex, std::pair<size_t, size_t> interval) {
  if (this->IsLeafIndex(bitIndex))
    return;

  assert(interval.first < interval.second);
  const auto &leftInterval = Util::GetLeftHalfInterval(interval);
  const auto &rightInterval = Util::GetRightHalfInterval(interval);
  this->bitsLeftLeafIndexIntervals[bitIndex] = leftInterval;
  this->bitsRightLeafIndexIntervals[bitIndex] = rightInterval;

  this->InitBitsLeafIndexIntervalsRecursively(this->GetLeftIndex(bitIndex),
                                              leftInterval);
  this->InitBitsLeafIndexIntervalsRecursively(this->GetRightIndex(bitIndex),
                                              rightInterval);
}

bool PLRUIndexTree::IsLeafIndex(size_t bitOrLeafIndex) const {
  return bitOrLeafIndex >= this->bits.size();
}

bool PLRUIndexTree::IsLeafIndexInLeft(size_t bitIndex, size_t leafIndex) const {
  assert(!this->IsLeafIndex(bitIndex));
  return Util::IsInInterval(leafIndex,
                            this->bitsLeftLeafIndexIntervals[bitIndex]);
}

bool PLRUIndexTree::IsLeafIndexInRight(size_t bitIndex,
                                       size_t leafIndex) const {
  assert(!this->IsLeafIndex(bitIndex));
  return Util::IsInInterval(leafIndex,
                            this->bitsRightLeafIndexIntervals[bitIndex]);
}

size_t PLRUIndexTree::GetLeftIndex(size_t bitIndex) const {
  return (2 * bitIndex) + 1;
}

size_t PLRUIndexTree::GetRightIndex(size_t bitIndex) const {
  return (2 * bitIndex) + 2;
}

size_t PLRUIndexTree::GetBitNodeParentIndex(size_t bitIndex) const {
  assert(bitIndex != 0);
  return (bitIndex - 1) / 2;
}

size_t PLRUIndexTree::GetLeafNodeParentIndex(size_t leafIndex) const {
  const auto bitIndex = leafIndex + this->bits.size();
  return this->GetBitNodeParentIndex(bitIndex);
}

void PLRUIndexTree::MarkLeafNodeAndItsAncestorsNonEmpty(size_t leafIndex) {
  const auto bitIndex = leafIndex + this->bits.size();
  this->MarkBranchAndParentBranchesNonEmpty(bitIndex);
}

void PLRUIndexTree::MarkBranchAndParentBranchesNonEmpty(size_t bitIndex) {
  if (this->isBranchNonEmpty[bitIndex])
    return;
  this->isBranchNonEmpty[bitIndex] = true;
  const auto isRootBit = (bitIndex == 0);
  if (!isRootBit)
    this->MarkBranchAndParentBranchesNonEmpty(
        this->GetBitNodeParentIndex(bitIndex));
}

void PLRUIndexTree::SwitchBitsOnPath(size_t leafIndex) {
  this->SwitchBitsOnPathRecursively(0, leafIndex);
}

void PLRUIndexTree::SwitchBitsOnPathRecursively(size_t bitIndex,
                                                size_t leafIndex) {
  if (this->IsLeafIndex(bitIndex))
    return;

  if (this->IsLeafIndexInLeft(bitIndex, leafIndex)) {
    this->bits[bitIndex] = RIGHT;
    this->SwitchBitsOnPathRecursively(this->GetLeftIndex(bitIndex), leafIndex);
  } else {
    assert(this->IsLeafIndexInRight(bitIndex, leafIndex));
    this->bits[bitIndex] = LEFT;
    this->SwitchBitsOnPathRecursively(this->GetRightIndex(bitIndex), leafIndex);
  }
}

size_t PLRUIndexTree::FindNewCacheLineIndexAndSwitchBitsOnPath() {
  return this->FindNewCacheLineIndexAndSwitchBitsOnPathRecursively(0);
}

size_t PLRUIndexTree::FindNewCacheLineIndexAndSwitchBitsOnPathRecursively(
    size_t bitIndex) {
  if (this->IsLeafIndex(bitIndex))
    return bitIndex - this->bits.size();

  if (this->bits[bitIndex] == LEFT) {
    this->bits[bitIndex] = RIGHT;
    return this->FindNewCacheLineIndexAndSwitchBitsOnPathRecursively(
        this->GetLeftIndex(bitIndex));
  } else {
    this->bits[bitIndex] = LEFT;
    return this->FindNewCacheLineIndexAndSwitchBitsOnPathRecursively(
        this->GetRightIndex(bitIndex));
  }
}

void PLRUIndexTree::AddLinesToCacheStateSnapshot(
    const std::vector<std::optional<CacheLine>> &leaves,
    CacheStateSnapshot &cacheStateSnapshot) const {
  this->AddLinesToCacheStateSnapshotRecursively(0, leaves, cacheStateSnapshot);
}

void PLRUIndexTree::AddLinesToCacheStateSnapshotRecursively(
    size_t bitIndex, const std::vector<std::optional<CacheLine>> &leaves,
    CacheStateSnapshot &cacheStateSnapshot) const {
  if (!this->isBranchNonEmpty[bitIndex])
    return;

  if (this->IsLeafIndex(bitIndex)) {
    const auto leafIndex = bitIndex - this->bits.size();
    this->AddLineToCacheStateSnapshot(leafIndex, leaves, cacheStateSnapshot);
    return;
  }

  const auto left = this->GetLeftIndex(bitIndex);
  const auto right = this->GetRightIndex(bitIndex);
  if (this->bits[bitIndex]) {
    this->AddLinesToCacheStateSnapshotRecursively(left, leaves,
                                                  cacheStateSnapshot);
    this->AddLinesToCacheStateSnapshotRecursively(right, leaves,
                                                  cacheStateSnapshot);
  } else {
    this->AddLinesToCacheStateSnapshotRecursively(right, leaves,
                                                  cacheStateSnapshot);
    this->AddLinesToCacheStateSnapshotRecursively(left, leaves,
                                                  cacheStateSnapshot);
  }
}

void PLRUIndexTree::AddLineToCacheStateSnapshot(
    size_t leafIndex, const std::vector<std::optional<CacheLine>> &leaves,
    CacheStateSnapshot &cacheStateSnapshot) const {
  const auto &cl = leaves[leafIndex];
  assert(cl);
  cacheStateSnapshot.AddLine(*cl, this->globalIteratorState);
}
