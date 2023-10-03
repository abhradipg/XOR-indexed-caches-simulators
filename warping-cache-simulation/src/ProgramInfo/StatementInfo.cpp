#include "StatementInfo.hpp"

const std::vector<AccessInfo> &StatementInfo::GetAccessInfos() const {
  return this->accessInfos;
}

const isl::set &StatementInfo::GetTimestampDomain() const {
  return this->timestampDomain;
}

const std::vector<std::string> &StatementInfo::GetTimestampStrDims() const {
  return this->timestampStrDims;
}

void StatementInfo::AddReadAccessInfo(const ArrayInfo &arrayInfo,
                                      const isl::map &taggedRead) {
  this->accessInfos.emplace_back(true, arrayInfo, taggedRead);
}

void StatementInfo::AddWriteAccessInfo(const ArrayInfo &arrayInfo,
                                       const isl::map &taggedWrite) {
  this->accessInfos.emplace_back(false, arrayInfo, taggedWrite);
}

void StatementInfo::AddTimestampInfo(const isl::map &timestampToStatement) {
  this->SetTimestampDomainAndStrDims(timestampToStatement.domain());
  this->RenameAccessInfosSymbolicVars(timestampToStatement);
}

void StatementInfo::SortAccessInfos() {
  std::sort(this->accessInfos.begin(), this->accessInfos.end());
}

void StatementInfo::SetTimestampDomainAndStrDims(const isl::set &timestampDom) {
  this->timestampDomain = timestampDom;
  assert(this->timestampStrDims.empty());
  const auto dim = IslUtil::GetSetDimension(timestampDom);
  this->timestampStrDims.resize(dim);
  const auto &dimsAsStrVec =
      IslUtil::ConvertSetTupleToStringVector(this->timestampDomain);
  assert(dimsAsStrVec.size() == dim);
  for (size_t pos = 0; pos < dim; pos++) {
    const auto &asStr = dimsAsStrVec[pos];
    if (StrUtil::IsInteger(asStr)) {
      this->timestampStrDims[pos] = asStr;
      continue;
    }
    const auto &dimName = "i" + std::to_string(pos);
    this->timestampStrDims[pos] = dimName;
  }
  assert(!this->timestampStrDims.empty());
}

void StatementInfo::RenameAccessInfosSymbolicVars(
    const isl::map &timestampToStatement) {
  for (auto &ai : this->accessInfos)
    ai.RenameSymbolicVars(
        IslUtil::ExtractSymbolicNameMapping(timestampToStatement));
}
