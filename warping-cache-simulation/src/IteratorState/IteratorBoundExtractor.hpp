#pragma once

#include <isl/cpp.h>
#include <memory>
#include <variant>

#include "Util/AffineFunctionBound.hpp"
#include "Util/Bound.hpp"
#include "Util/ConstantBound.hpp"
#include "Util/IslUtil.hpp"

class IteratorBoundExtractor {
public:
  using ItIdMap = const std::unordered_map<std::string, size_t> &;

  IteratorBoundExtractor(const isl::set &domain,
                         const std::string &itUniqueName, ItIdMap itIdMap);

  [[nodiscard]] std::unique_ptr<Bound> GetLowerBound() const;

  [[nodiscard]] std::unique_ptr<Bound> GetUpperBound() const;

  [[nodiscard]] bool IsIncreasing() const;

private:
  struct BoundInfo {
    long constantLower = std::numeric_limits<long>::min();
    long constantUpper = std::numeric_limits<long>::max();
    isl::aff affLower;
    isl::aff affUpper;
    bool foundConstantLower = false;
    bool foundConstantUpper = false;
    size_t dim;
    long stride;
    isl::point samplePoint;
    BoundInfo(size_t dim, long stride, const isl::point &samplePoint)
        : dim(dim), stride(stride), samplePoint(samplePoint) {}
  };

  std::variant<long, std::unique_ptr<AffineFunction>> lowerBound;
  std::variant<long, std::unique_ptr<AffineFunction>> upperBound;
  long stride;
};
