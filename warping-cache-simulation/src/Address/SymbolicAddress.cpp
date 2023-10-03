#include "SymbolicAddress.hpp"
#include "Util/IslUtil.hpp"

SymbolicAddress::SymbolicAddress(const isl::aff &aff,
                                 const std::unordered_map<std::string, size_t>
                                     &iteratorUniqueNamesToIteratorIds)
    : affineFunction(aff, iteratorUniqueNamesToIteratorIds),
      hashWithoutConstant(this->ComputeHashWithoutConstant()) {}

long SymbolicAddress::GetCoefficient(size_t itId) const {
  return this->affineFunction.GetCoefficient(itId);
}

const std::unordered_map<size_t, long> &
SymbolicAddress::GetItIdsToCoefficients() const {
  return this->affineFunction.GetVarIdsToCoefficients();
}

long SymbolicAddress::Eval(
    const std::unordered_map<size_t, const std::shared_ptr<long>>
        &currentIteratorValuePointers) const {
  return this->affineFunction.Eval(currentIteratorValuePointers);
}

long SymbolicAddress::Eval(
    const GlobalIteratorState &globalIteratorState) const {
  return this->affineFunction.Eval(globalIteratorState);
}

size_t SymbolicAddress::ComputeHash(
    size_t concreteAddress,
    const GlobalIteratorState &globalIteratorState) const {
  size_t hash = this->hashWithoutConstant;
  const auto shiftedConstantHash =
      this->ComputeShiftedConstantHash(concreteAddress, globalIteratorState);
  Util::HashCombine(shiftedConstantHash, hash);
  return hash;
}

std::string SymbolicAddress::ToString() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

size_t SymbolicAddress::ComputeHashWithoutConstant() const {
  size_t hash = this->affineFunction.GetVarIdsToCoefficients().size();
  for (const auto itId : this->affineFunction.GetSortedVarIds()) {
    const auto coefficient = affineFunction.GetCoefficient(itId);
    assert(coefficient != 0);
    const auto itHash = this->ComputeItIdAndCoefficientHash(itId, coefficient);
    Util::HashCombine(itHash, hash);
  }
  return hash;
}

size_t SymbolicAddress::ComputeItIdAndCoefficientHash(size_t itId,
                                                      long coefficient) const {
  assert(coefficient != 0);
  auto hash = std::hash<size_t>()(itId);
  Util::HashCombine(std::hash<long>()(coefficient), hash);
  return hash;
}

size_t SymbolicAddress::ComputeShiftedConstantHash(
    size_t concreteAddress,
    const GlobalIteratorState &globalIteratorState) const {
  const auto mappingConcreteAddress = this->Eval(globalIteratorState);
  const auto diff = (long)mappingConcreteAddress - (long)concreteAddress;
  const auto shiftedConstant = this->affineFunction.GetConstant() - diff;
  return std::hash<long>()(shiftedConstant);
}

std::ostream &operator<<(std::ostream &os, const SymbolicAddress &sa) {
  os << sa.affineFunction;
  return os;
}
