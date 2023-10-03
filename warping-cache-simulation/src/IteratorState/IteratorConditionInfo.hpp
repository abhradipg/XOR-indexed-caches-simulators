#pragma once

#include <optional>

#include "IteratorState/IteratorBound.hpp"
#include "IteratorState/IteratorStateMap.hpp"

class IteratorConditionInfo {
public:
  IteratorConditionInfo(size_t itId, const isl::basic_set &conditionDomain,
                        const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool DoesConditionCurrentlyFail() const;

private:
  const IteratorStateMap &iteratorStateMap;
  const IteratorState &iteratorState;

  // TODO: extract equality conditions
  // TODO: extract mod conditions

  const std::optional<long> constLowerBound;
  const std::optional<long> constUpperBound;
  const std::vector<AffineFunction> affineLowerBounds;
  const std::vector<AffineFunction> affineUpperBounds;

  [[nodiscard]] bool DoesFailConstLowerBound() const;
  [[nodiscard]] bool DoesFailConstUpperBound() const;
  [[nodiscard]] bool DoesFailAnyAffLowerBound() const;
  [[nodiscard]] bool DoesFailAnyAffUpperBound() const;

  [[nodiscard]] static std::optional<long>
  ExtractConstLowerBound(const isl::basic_set &conditionDomain,
                         const std::string &dimName);

  [[nodiscard]] static std::optional<long>
  ExtractConstUpperBound(const isl::basic_set &conditionDomain,
                         const std::string &dimName);

  [[nodiscard]] std::vector<AffineFunction>
  ExtractAffineLowerBounds(const isl::basic_set &conditionDomain,
                           const std::string &dimName) const;

  [[nodiscard]] std::vector<AffineFunction>
  ExtractAffineUpperBounds(const isl::basic_set &conditionDomain,
                           const std::string &dimName) const;
};
