#pragma once

#include <cassert>
#include <functional>
#include <isl/cpp.h>
#include <set>
#include <string>
#include <unordered_map>

#include "IteratorState/GlobalIteratorState.hpp"
#include "Util/AffineFunction.hpp"

class SymbolicAddress {
public:
  SymbolicAddress(const isl::aff &aff,
                  const std::unordered_map<std::string, size_t>
                      &iteratorUniqueNamesToIteratorIds);

  [[nodiscard]] long GetCoefficient(size_t itId) const;

  [[nodiscard]] const std::unordered_map<size_t, long> &
  GetItIdsToCoefficients() const;

  [[nodiscard]] long
  Eval(const std::unordered_map<size_t, const std::shared_ptr<long>>
           &currentIteratorValuePointers) const;

  [[nodiscard]] long Eval(const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] size_t
  ComputeHash(size_t concreteAddress,
              const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] std::string ToString() const;

private:
  const AffineFunction affineFunction;
  const size_t hashWithoutConstant;

  [[nodiscard]] size_t ComputeHashWithoutConstant() const;

  [[nodiscard]] size_t ComputeItIdAndCoefficientHash(size_t itId,
                                                     long coefficient) const;

  [[nodiscard]] size_t ComputeShiftedConstantHash(
      size_t concreteAddress,
      const GlobalIteratorState &globalIteratorState) const;

  friend std::ostream &operator<<(std::ostream &os, const SymbolicAddress &sa);
};
