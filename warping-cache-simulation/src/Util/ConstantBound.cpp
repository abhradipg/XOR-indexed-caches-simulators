#include "ConstantBound.hpp"

ConstantBound::ConstantBound(long val) : val(val) {}

long ConstantBound::GetCurrentValue(
    const std::unordered_map<size_t, const std::shared_ptr<long>> &) const {
  return this->val;
}

bool ConstantBound::IsConstant() const { return true; }

std::set<size_t> ConstantBound::GetAffectingVarIds() const { return {}; }
