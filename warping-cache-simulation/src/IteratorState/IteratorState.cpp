#include "IteratorState.hpp"

#include <utility>

IteratorState::IteratorState(IteratorInfo iteratorInfo,
                             IteratorBound iteratorBound, IteratorStateMap &ism)
    : iteratorInfo(std::move(iteratorInfo)),
      iteratorBound(std::move(iteratorBound)) {
  this->isWarpingApplicable = true;
  if (!this->iteratorBound.HasConstantNumberOfIterations())
    this->MarkParentsAffectingBoundAsWarpingInapplicable(ism);

  this->currentValuePointer = std::make_shared<long>();
  this->Refresh(ism);
}

size_t IteratorState::GetIteratorId() const {
  return this->iteratorInfo.GetId();
}

const std::string &IteratorState::GetIteratorName() const {
  return this->iteratorInfo.GetName();
}

const std::string &IteratorState::GetIteratorUniqueName() const {
  return this->iteratorInfo.GetUniqueName();
}

size_t IteratorState::GetNumberOfIterations() const {
  return this->nIterations;
}

const std::shared_ptr<long> &IteratorState::GetCurrentValuePointer() const {
  return this->currentValuePointer;
}

long IteratorState::GetCurrentValue() const {
  return *this->currentValuePointer;
}

long IteratorState::GetFirstValue() const { return this->firstValue; }

long IteratorState::GetLastValue() const { return this->lastValue; }

long IteratorState::GetStride() const { return this->iteratorInfo.GetStride(); }

bool IteratorState::IsFinished() const {
  return this->nthIteration > this->nIterations;
}

bool IteratorState::IsIncreasing() const { return this->GetStride() > 0; }

size_t IteratorState::GetNthIteration() const {
  assert(this->nthIteration ==
         (size_t)((*this->currentValuePointer - this->firstValue) /
                  this->GetStride()) +
             1);
  return this->nthIteration;
}

size_t IteratorState::GetNumberOfRemainingIterations() const {
  auto result = this->nIterations - this->nthIteration + 1;
  assert(result == this->GetNumberOfIterationsAfterIteratorValue(
                       this->GetCurrentValue()));
  return result;
}

size_t
IteratorState::GetNumberOfIterationsAfterIteratorValue(long value) const {
  return this->GetNumberOfIterationsBetweenIteratorValues(
      value, this->GetLastValue() + this->GetStride());
}

size_t
IteratorState::GetNumberOfIterationsUntilIteratorValue(long value) const {
  return this->GetNumberOfIterationsBetweenIteratorValues(
      this->GetCurrentValue(), value);
}

size_t
IteratorState::GetNumberOfIterationsBetweenIteratorValues(long value1,
                                                          long value2) const {
  auto distance = std::abs(value2 - value1);
  assert(distance % this->GetStride() == 0);
  return distance / std::abs(this->GetStride());
}

bool IteratorState::IsWarpingApplicable() const {
  return this->isWarpingApplicable;
}

void IteratorState::Advance() {
  assert(this->GetNumberOfRemainingIterations() > 0);
  assert(!this->IsFinished());
  this->nthIteration += 1;
  *this->currentValuePointer += this->GetStride();
}

void IteratorState::Warp(const WarpInfo &warpInfo) {
  this->AdvanceMultipleTimes(warpInfo.GetNItAdvance());
}

void IteratorState::AdvanceMultipleTimes(size_t times) {
  this->nthIteration += times;
  *this->currentValuePointer += (long)times * this->GetStride();
}

void IteratorState::Refresh(const IteratorStateMap &ism) {
  const auto &firstAndLastVal = this->iteratorBound.GetFirstAndLastValue(
      ism.GetCurrentIteratorValuePointers());
  this->firstValue = firstAndLastVal.first;
  this->lastValue = firstAndLastVal.second;
  this->nthIteration = 1;
  this->nIterations =
      ((this->lastValue - this->firstValue) / this->GetStride()) + 1;
  *this->currentValuePointer = this->firstValue;
}

void IteratorState::MarkAsWarpingInapplicable() {
  this->isWarpingApplicable = false;
}

void IteratorState::MarkParentsAffectingBoundAsWarpingInapplicable(
    IteratorStateMap &ism) {
  if (!this->iteratorBound.HasConstantNumberOfIterations()) {
    for (const auto id : this->iteratorBound.GetAffectingItIds()) {
      assert(id != this->GetIteratorId());
      ism.MarkAsWarpingInapplicable(id);
    }
  }
}
