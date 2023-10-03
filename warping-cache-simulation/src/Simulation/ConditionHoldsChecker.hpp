#pragma once

#include <isl/cpp.h>
#include <string>

#include "IteratorState/IteratorConditionInfo.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Util/IslUtil.hpp"

class ConditionHoldsChecker {
public:
  ConditionHoldsChecker(std::set<size_t> conditionIteratorIds,
                        const isl::set &conditionDomain,
                        const IteratorStateMap &iteratorStateMap);

  [[nodiscard]] bool DoesConditionHold() const;

private:
  const IteratorStateMap &iteratorStateMap;

  const std::set<size_t> conditionIteratorIds;

  const isl::set conditionDomain;

  std::unordered_map<size_t, IteratorConditionInfo> iteratorConditionInfos;

  isl::set parametrizedConditionHoldsQuery;
  isl::basic_set conditionHoldsQueryParams;

  [[nodiscard]] bool DoesConditionHoldAccordingToIslQuery() const;
  [[nodiscard]] bool DoesConditionHoldAccordingToItConditionInfos() const;

  void InitIteratorConditionInfos();
  void InitParametrizedConditionHoldsQuery();
  void InitConditionHoldsQueryParams();

  [[nodiscard]] std::string ConstructParametrizedConditionHoldsQueryStr() const;
  [[nodiscard]] std::string ConstructConditionHoldsQueryParamsStr() const;
  [[nodiscard]] std::string ConstructConditionHoldsConditionStr() const;
  [[nodiscard]] std::string ConstructConditionHoldsParamStr() const;
  [[nodiscard]] std::string
  ConstructConditionHoldsQueryItParamName(size_t itId) const;
};
