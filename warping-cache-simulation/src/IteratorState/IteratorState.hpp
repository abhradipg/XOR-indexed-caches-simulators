#pragma once

#include <cassert>
#include <isl/cpp.h>
#include <memory>
#include <string>
#include <variant>

#include "IteratorState/IteratorBound.hpp"
#include "IteratorState/IteratorInfo.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Util/IslUtil.hpp"
#include "Warping/WarpInfo.hpp"

class IteratorStateMap;

class IteratorState {

public:
  // TODO: refactor, encapsulate bound stuff, encapsulate other stuff...

  IteratorState(IteratorInfo iteratorInfo, IteratorBound iteratorBound,
                IteratorStateMap &ism);

  [[nodiscard]] size_t GetIteratorId() const;

  [[nodiscard]] const std::string &GetIteratorName() const;
  [[nodiscard]] const std::string &GetIteratorUniqueName() const;

  [[nodiscard]] const std::shared_ptr<long> &GetCurrentValuePointer() const;

  [[nodiscard]] long GetCurrentValue() const;
  [[nodiscard]] long GetFirstValue() const;
  [[nodiscard]] long GetLastValue() const;

  [[nodiscard]] long GetStride() const;

  [[nodiscard]] bool IsIncreasing() const;

  [[nodiscard]] bool IsFinished() const;

  [[nodiscard]] size_t GetNthIteration() const;
  [[nodiscard]] size_t GetNumberOfIterations() const;
  [[nodiscard]] size_t GetNumberOfRemainingIterations() const;
  [[nodiscard]] size_t
  GetNumberOfIterationsAfterIteratorValue(long value) const;
  [[nodiscard]] size_t
  GetNumberOfIterationsUntilIteratorValue(long value) const;
  [[nodiscard]] size_t
  GetNumberOfIterationsBetweenIteratorValues(long value1, long value2) const;
  [[nodiscard]] bool IsWarpingApplicable() const;

  void Advance();
  void Warp(const WarpInfo &warpInfo);
  void Refresh(const IteratorStateMap &ism);

  void MarkAsWarpingInapplicable();

private:
  IteratorInfo iteratorInfo;
  IteratorBound iteratorBound;

  size_t nIterations;
  size_t nthIteration;
  std::shared_ptr<long> currentValuePointer;
  long lastValue;
  long firstValue;

  bool isWarpingApplicable;

  void AdvanceMultipleTimes(size_t times);

  void MarkParentsAffectingBoundAsWarpingInapplicable(IteratorStateMap &ism);
};
