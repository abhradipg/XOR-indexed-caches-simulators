#include "AffineFunction.hpp"
AffineFunction::AffineFunction(
    const isl::aff &aff,
    const std::unordered_map<std::string, size_t> &varNamesToVarIds)
    : constant(this->ExtractConstant(aff)),
      varIdsToCoefficients(
          this->ExtractVarIdsToCoefficients(aff, varNamesToVarIds)),
      varIdsToVarNames(this->ExtractVarIdsToVarNames(varNamesToVarIds)),
      sortedVarIds(this->ExtractSortedVarIds()) {}

long AffineFunction::GetConstant() const { return this->constant; }

long AffineFunction::GetCoefficient(size_t varId) const {
  const auto it = this->varIdsToCoefficients.find(varId);
  if (it == this->varIdsToCoefficients.end())
    return 0;
  return it->second;
}

const std::unordered_map<size_t, long> &
AffineFunction::GetVarIdsToCoefficients() const {
  return this->varIdsToCoefficients;
}

const std::set<size_t> &AffineFunction::GetSortedVarIds() const {
  return this->sortedVarIds;
}

long AffineFunction::Eval(
    const std::unordered_map<size_t, const std::shared_ptr<long>>
        &varIdToVarValPointers) const {
  long result = this->constant;
  for (const auto &el : this->varIdsToCoefficients) {
    const auto varId = el.first;
    const auto coefficient = el.second;
    const auto it = varIdToVarValPointers.find(varId);
    assert(it != varIdToVarValPointers.end());
    assert(it->second != nullptr);
    result += coefficient * (*it->second);
  }
  return result;
}

long AffineFunction::Eval(
    const GlobalIteratorState &globalIteratorState) const {
  long result = this->constant;
  for (const auto &el : this->varIdsToCoefficients) {
    const auto varId = el.first;
    const auto varVal = globalIteratorState.GetIteratorValue(varId);
    const auto coefficient = el.second;
    result += coefficient * (varVal);
  }
  return result;
}

std::string AffineFunction::ToString() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

long AffineFunction::ExtractConstant(const isl::aff &aff) const {
  return IslUtil::GetAffConstant(aff);
}

std::set<size_t> AffineFunction::ExtractSortedVarIds() const {
  std::set<size_t> res;
  for (const auto &el : this->varIdsToCoefficients)
    res.insert(el.first);
  return res;
}

std::unordered_map<size_t, long> AffineFunction::ExtractVarIdsToCoefficients(
    const isl::aff &aff,
    const std::unordered_map<std::string, size_t> &varNamesToVarIds) const {
  std::unordered_map<size_t, long> res;
  const auto nDims = IslUtil::GetAffDomainDimension(aff);
  for (size_t dim = 0; dim < nDims; dim++) {
    const auto coefficient = IslUtil::GetAffDomainDimCoefficient(aff, dim);
    if (coefficient != 0) {
      const auto &varName = IslUtil::GetAffDomainDimName(aff, dim);
      assert(varNamesToVarIds.count(varName) == 1);
      const auto varId = varNamesToVarIds.at(varName);
      assert(res.count(varId) == 0);
      res.emplace(varId, coefficient);
    }
  }
  return res;
}

std::unordered_map<size_t, std::string> AffineFunction::ExtractVarIdsToVarNames(
    const std::unordered_map<std::string, size_t> &varNamesToVarIds) const {
  std::unordered_map<size_t, std::string> res;
  for (const auto &el : varNamesToVarIds) {
    const auto &varName = el.first;
    const auto varId = el.second;
    assert(res.count(varId) == 0);
    res.emplace(varId, varName);
  }
  return res;
}

std::ostream &operator<<(std::ostream &os, const AffineFunction &af) {
  for (const auto &el : af.varIdsToCoefficients) {
    const auto varId = el.first;
    const auto varCoefficient = el.second;
    const auto &varName = af.varIdsToVarNames.at(varId);
    assert(!varName.empty());
    os << varCoefficient << "*(" << varName << ")+";
  }
  os << "(" << af.constant << ")";
  return os;
}
