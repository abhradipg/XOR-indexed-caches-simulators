#pragma once

#include <cassert>
#include <functional>
#include <isl/cpp.h>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include "IteratorState/GlobalIteratorState.hpp"
#include "Util/IslUtil.hpp"

class AffineFunction {
public:
  AffineFunction(
      const isl::aff &aff,
      const std::unordered_map<std::string, size_t> &varNamesToVarIds);

  [[nodiscard]] long GetConstant() const;

  [[nodiscard]] long GetCoefficient(size_t varId) const;

  [[nodiscard]] const std::unordered_map<size_t, long> &
  GetVarIdsToCoefficients() const;

  [[nodiscard]] const std::set<size_t> &GetSortedVarIds() const;

  [[nodiscard]] long
  Eval(const std::unordered_map<size_t, const std::shared_ptr<long>>
           &varIdToVarValPointers) const;

  [[nodiscard]] long Eval(const GlobalIteratorState &globalIteratorState) const;

  [[nodiscard]] std::string ToString() const;

private:
  const long constant;
  const std::unordered_map<size_t, long> varIdsToCoefficients;
  const std::unordered_map<size_t, std::string> varIdsToVarNames;
  const std::set<size_t> sortedVarIds;

  [[nodiscard]] long ExtractConstant(const isl::aff &aff) const;

  [[nodiscard]] std::unordered_map<size_t, long> ExtractVarIdsToCoefficients(
      const isl::aff &aff,
      const std::unordered_map<std::string, size_t> &varNamesToVarIds) const;

  [[nodiscard]] std::unordered_map<size_t, std::string> ExtractVarIdsToVarNames(
      const std::unordered_map<std::string, size_t> &varNamesToVarIds) const;

  [[nodiscard]] std::set<size_t> ExtractSortedVarIds() const;

  friend std::ostream &operator<<(std::ostream &os, const AffineFunction &af);
};
