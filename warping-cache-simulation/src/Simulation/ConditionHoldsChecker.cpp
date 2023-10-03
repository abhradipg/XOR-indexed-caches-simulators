#include "ConditionHoldsChecker.hpp"
ConditionHoldsChecker::ConditionHoldsChecker(

    std::set<size_t> conditionIteratorIds, const isl::set &conditionDomain,
    const IteratorStateMap &iteratorStateMap)
    : iteratorStateMap(iteratorStateMap),
      conditionIteratorIds(std::move(conditionIteratorIds)),
      conditionDomain(conditionDomain) {
  this->InitIteratorConditionInfos();
  this->InitParametrizedConditionHoldsQuery();
  this->InitConditionHoldsQueryParams();
}

bool ConditionHoldsChecker::DoesConditionHold() const {
  return this->DoesConditionHoldAccordingToItConditionInfos();
  // return this->DoesConditionHoldAccordingToIslQuery();
}

bool ConditionHoldsChecker::DoesConditionHoldAccordingToIslQuery() const {
  // TODO: this is currently not working, FIX
  std::vector<long> paramVals;
  for (const auto itId : this->conditionIteratorIds)
    paramVals.push_back(this->iteratorStateMap.GetIteratorCurrentValue(itId));
  const auto &params = IslUtil::SetBasicSetParamValues(
      this->conditionHoldsQueryParams, paramVals);
  const auto &query = this->conditionHoldsQueryParams.intersect_params(params);
  return !query.is_empty();
}

bool ConditionHoldsChecker::DoesConditionHoldAccordingToItConditionInfos()
    const {
  for (const auto &el : this->iteratorConditionInfos)
    if (el.second.DoesConditionCurrentlyFail())
      return false;
  return true;
}

void ConditionHoldsChecker::InitIteratorConditionInfos() {
  const auto &bSetDom = IslUtil::ConvertSetToBasicSet(this->conditionDomain);
  for (const auto itId : this->conditionIteratorIds)
    this->iteratorConditionInfos.emplace(
        itId, IteratorConditionInfo(itId, bSetDom, this->iteratorStateMap));
}

void ConditionHoldsChecker::InitParametrizedConditionHoldsQuery() {
  const auto &str = this->ConstructParametrizedConditionHoldsQueryStr();
  this->parametrizedConditionHoldsQuery =
      isl::set(this->conditionDomain.ctx(), str);
}

void ConditionHoldsChecker::InitConditionHoldsQueryParams() {
  const auto &str = this->ConstructConditionHoldsQueryParamsStr();
  this->conditionHoldsQueryParams =
      isl::basic_set(this->conditionDomain.ctx(), str);
}

std::string
ConditionHoldsChecker::ConstructParametrizedConditionHoldsQueryStr() const {
  constexpr auto format = "[%s] -> { %s }";
  const auto &paramStr = this->ConstructConditionHoldsParamStr();
  const auto &condStr = this->ConstructConditionHoldsConditionStr();
  return StrUtil::Format(format, paramStr.c_str(), condStr.c_str());
}

std::string
ConditionHoldsChecker::ConstructConditionHoldsQueryParamsStr() const {
  constexpr auto format = "[%s] -> {:}";
  const auto &paramStr = this->ConstructConditionHoldsParamStr();
  return StrUtil::Format(format, paramStr.c_str());
}

std::string ConditionHoldsChecker::ConstructConditionHoldsParamStr() const {
  std::string paramStr;
  std::vector<size_t> itIds = {this->conditionIteratorIds.begin(),
                               this->conditionIteratorIds.end()};
  for (size_t i = 0; i < itIds.size(); i++) {
    paramStr.append(this->ConstructConditionHoldsQueryItParamName(itIds[i]));
    if (i != itIds.size() - 1)
      paramStr.append(", ");
  }
  return paramStr;
}

std::string ConditionHoldsChecker::ConstructConditionHoldsQueryItParamName(
    size_t itId) const {
  return "V_" + this->iteratorStateMap.GetIteratorUniqueName(itId);
}

std::string ConditionHoldsChecker::ConstructConditionHoldsConditionStr() const {
  std::string paramCondStr =
      IslUtil::GetStringWithoutBraces(this->conditionDomain);
  for (const auto itId : this->conditionIteratorIds) {
    const auto &itUniqueName =
        this->iteratorStateMap.GetIteratorUniqueName(itId);
    const auto &cond = itUniqueName + " = " +
                       this->ConstructConditionHoldsQueryItParamName(itId);
    paramCondStr.append(" and ").append(cond);
  }
  return paramCondStr;
}
