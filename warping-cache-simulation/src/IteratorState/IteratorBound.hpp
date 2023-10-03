#pragma once

#include <algorithm>
#include <variant>

#include "IteratorState/IteratorBoundExtractor.hpp"
#include "Util/AffineFunction.hpp"

class IteratorBound {

public:
  using ItIdMap = const std::unordered_map<std::string, size_t> &;
  using ItValMap =
      const std::unordered_map<size_t, const std::shared_ptr<long>> &;

  IteratorBound(const isl::set &set, const std::string &itUniqueName,
                ItIdMap itIdMap);

  [[nodiscard]] bool HasConstantNumberOfIterations() const;

  [[nodiscard]] std::set<size_t> GetAffectingItIds() const;

  [[nodiscard]] std::pair<long, long>
  GetFirstAndLastValue(ItValMap itValMap) const;

  [[nodiscard]] std::pair<long, long>
  GetMinAndMaxValue(ItValMap itValMap) const;

private:
  std::unique_ptr<Bound> lowerBound;
  std::unique_ptr<Bound> upperBound;
  const bool isIncreasing;

  explicit IteratorBound(const IteratorBoundExtractor &ibe);
};
