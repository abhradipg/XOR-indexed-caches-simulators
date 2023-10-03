#include "IteratorBoundExtractor.hpp"

IteratorBoundExtractor::IteratorBoundExtractor(const isl::set &domain,
                                               const std::string &itUniqueName,
                                               ItIdMap itIdMap) {
  // TODO: REFACTOR, too long
  const auto dim = IslUtil::GetSetDimNamePos(domain, itUniqueName);
  this->stride = IslUtil::GetSetDimStride(domain, dim);
  auto reducedSet = IslUtil::EliminateDimsAfterDim(domain, dim);
  auto basicSet = isl::manage_copy(reducedSet.unshifted_simple_hull().get());
  auto extractBounds = [](isl_constraint *c, void *user) {
    auto bi = (BoundInfo *)user;
    auto isLowerBound = IslUtil::IsConstraintLowerBound(c, bi->dim);
    auto isUpperBound = IslUtil::IsConstraintUpperBound(c, bi->dim);

    if (!isLowerBound && !isUpperBound) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }
    const auto &name =
        std::string(isl_constraint_get_dim_name(c, isl_dim_set, bi->dim));
    const auto &bound = IslUtil::GetConstraintBound(c, bi->dim);
    const auto &boundStr = IslUtil::ToString(bound);
    const auto arrowPos = boundStr.find("->");

    assert(arrowPos != std::string::npos);
    const auto isSelfReferring =
        (boundStr.find(name + " ", arrowPos) != std::string::npos ||
         boundStr.find(name + ")", arrowPos) != std::string::npos);

    if (isSelfReferring) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }

    const auto isMod = (boundStr.find("mod", arrowPos) != std::string::npos);

    if (isMod) {
      assert(bi->stride != 1);
      isl_constraint_free(c);
      return isl_stat_ok;
    }

    const auto handleConstantBound = [&]() {
      const auto constantBound = bound.eval(bi->samplePoint).num_si();
      if (isLowerBound) {
        bi->constantLower = constantBound > bi->constantLower
                                ? constantBound
                                : bi->constantLower;
        bi->foundConstantLower = true;
      } else {
        assert(isUpperBound);
        bi->constantUpper = constantBound < bi->constantUpper
                                ? constantBound
                                : bi->constantUpper;
        bi->foundConstantUpper = true;
      }
    };

    const auto handleAffineBound = [&]() {
      const auto foundConstant =
          isLowerBound ? bi->foundConstantLower : bi->foundConstantUpper;
      if (foundConstant) {
        return;
      }

      auto &affineBound = isLowerBound ? bi->affLower : bi->affUpper;
      assert(affineBound.is_null());
      affineBound = bound;
    };

    if (IslUtil::IsAffConstant(bound))
      handleConstantBound();
    else
      handleAffineBound();

    isl_constraint_free(c);
    return isl_stat_ok;
  };

  auto bi = new BoundInfo(dim, this->stride, domain.sample_point());
  isl_basic_set_foreach_constraint(basicSet.get(), extractBounds, (void *)bi);

  if (bi->foundConstantLower) {
    this->lowerBound = bi->constantLower;
  } else {
    assert(!bi->affLower.is_null());
    this->lowerBound = std::make_unique<AffineFunction>(bi->affLower, itIdMap);
  }

  if (bi->foundConstantUpper) {
    this->upperBound = bi->constantUpper;
  } else {
    assert(!bi->affUpper.is_null());
    this->upperBound = std::make_unique<AffineFunction>(bi->affUpper, itIdMap);
  }

  delete bi;
}

std::unique_ptr<Bound> IteratorBoundExtractor::GetLowerBound() const {
  if (this->lowerBound.index() == 0)
    return std::make_unique<ConstantBound>(std::get<0>(this->lowerBound));
  return std::make_unique<AffineFunctionBound>(*std::get<1>(this->lowerBound));
}

std::unique_ptr<Bound> IteratorBoundExtractor::GetUpperBound() const {
  if (this->upperBound.index() == 0)
    return std::make_unique<ConstantBound>(std::get<0>(this->upperBound));
  return std::make_unique<AffineFunctionBound>(*std::get<1>(this->upperBound));
}

bool IteratorBoundExtractor::IsIncreasing() const { return this->stride > 0; }