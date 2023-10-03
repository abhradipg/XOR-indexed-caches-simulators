#pragma once

#include <isl/cpp.h>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "AccessInfo.hpp"
#include "CacheState/CacheAccess.hpp"
#include "Util/IslUtil.hpp"
#include "Util/PetUtil.hpp"

class StatementInfo {
public:
  [[nodiscard]] const isl::set &GetTimestampDomain() const;

  [[nodiscard]] const std::vector<std::string> &GetTimestampStrDims() const;

  [[nodiscard]] const std::vector<AccessInfo> &GetAccessInfos() const;

  void AddReadAccessInfo(const ArrayInfo &arrayInfo,
                         const isl::map &taggedRead);

  void AddWriteAccessInfo(const ArrayInfo &arrayInfo,
                          const isl::map &taggedWrite);

  void AddTimestampInfo(const isl::map &timestampToStatement);

  void SortAccessInfos();

private:
  isl::set timestampDomain;
  std::vector<std::string> timestampStrDims;
  std::vector<AccessInfo> accessInfos;

  void SetTimestampDomainAndStrDims(const isl::set &timestampDom);

  void RenameAccessInfosSymbolicVars(const isl::map &timestampToStatement);
};
