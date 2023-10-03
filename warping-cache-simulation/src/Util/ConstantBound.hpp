#pragma once

#include "Bound.hpp"

class ConstantBound : public Bound {
public:
  explicit ConstantBound(long val);

  [[nodiscard]] long GetCurrentValue(
      const std::unordered_map<size_t, const std::shared_ptr<long>> &)
      const override;

  [[nodiscard]] bool IsConstant() const override;

  [[nodiscard]] std::set<size_t> GetAffectingVarIds() const override;

private:
  const long val;
};
