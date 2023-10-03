#pragma once

#include <utility>

#include "Bound.hpp"
#include "Util/AffineFunction.hpp"

class AffineFunctionBound : public Bound {
public:
  explicit AffineFunctionBound(AffineFunction affineFunction);

  AffineFunctionBound(
      const isl::aff &aff,
      const std::unordered_map<std::string, size_t> &varNamesToVarIds);

  [[nodiscard]] long
  GetCurrentValue(const std::unordered_map<size_t, const std::shared_ptr<long>>
                      &varIdToVarValPointers) const override;

  [[nodiscard]] bool IsConstant() const override;

  [[nodiscard]] std::set<size_t> GetAffectingVarIds() const override;

private:
  const AffineFunction affineFunction;
};
