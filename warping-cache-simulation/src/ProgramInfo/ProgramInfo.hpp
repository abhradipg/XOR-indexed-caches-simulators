#pragma once

#include <iostream>
#include <isl/cpp.h>
#include <isl/set.h>
#include <list>
#include <map>
#include <memory>
#include <pet.h>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "CacheState/CacheAccess.hpp"
#include "ProgramInfo/AccessInfo.hpp"
#include "ProgramInfo/ArrayInfo.hpp"
#include "ProgramInfo/ArrayInfoExtractor.hpp"
#include "ProgramInfo/StatementInfo.hpp"
#include "Util/IslUtil.hpp"
#include "Util/StrUtil.hpp"
#include "Util/Util.hpp"

class ProgramInfo {
public:
  ProgramInfo(isl::ctx context, const std::string &source);

  [[nodiscard]] const std::map<std::string, StatementInfo> &
  GetStatementInfos() const;

private:
  std::map<std::string, StatementInfo> statementInfos;

  void InitStatementInfos(pet_scop *scop);

  void AddStatementInfosAccessInfos(pet_scop *scop);

  void AddStatementInfosReadAccessInfos(
      const isl::union_map &taggedReads,
      const std::unordered_map<std::string, ArrayInfo> &arrayInfos);

  void AddStatementInfosWriteAccessInfos(
      const isl::union_map &taggedWrites,
      const std::unordered_map<std::string, ArrayInfo> &arrayInfos);

  void AddStatementInfosTimestampInfos(pet_scop *scop);

  void SortStatementInfosAccessInfos();

  [[nodiscard]] std::unordered_map<std::string, ArrayInfo>
  ExtractArrayInfos(pet_scop *scop) const;

  [[nodiscard]] isl::union_map
  ExtractTimestampToStatement(pet_scop *scop) const;

  void PrintArrayInfos(
      const std::unordered_map<std::string, ArrayInfo> &arrayInfos) const;

  void PrintStatementInfos() const;
};
