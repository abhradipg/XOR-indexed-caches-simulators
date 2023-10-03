#pragma once

#include <memory>
#include <set>
#include <unordered_map>

class Bound {
public:
  [[nodiscard]] virtual long
  GetCurrentValue(const std::unordered_map<size_t, const std::shared_ptr<long>>
                      &varIdToVarValPointers) const = 0;

  [[nodiscard]] virtual bool IsConstant() const = 0;

  [[nodiscard]] virtual std::set<size_t> GetAffectingVarIds() const = 0;
};
