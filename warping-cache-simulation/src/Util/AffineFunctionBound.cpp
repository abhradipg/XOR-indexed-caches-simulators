#include "AffineFunctionBound.hpp"

AffineFunctionBound::AffineFunctionBound(AffineFunction affineFunction)
    : affineFunction(std::move(affineFunction)) {}

AffineFunctionBound::AffineFunctionBound(
    const isl::aff &aff,
    const std::unordered_map<std::string, size_t> &varNamesToVarIds)
    : affineFunction(aff, varNamesToVarIds) {}

long AffineFunctionBound::GetCurrentValue(
    const std::unordered_map<size_t, const std::shared_ptr<long>>
        &varIdToVarValPointers) const {
  return this->affineFunction.Eval(varIdToVarValPointers);
}

bool AffineFunctionBound::IsConstant() const { return false; }

std::set<size_t> AffineFunctionBound::GetAffectingVarIds() const {
  return this->affineFunction.GetSortedVarIds();
}
