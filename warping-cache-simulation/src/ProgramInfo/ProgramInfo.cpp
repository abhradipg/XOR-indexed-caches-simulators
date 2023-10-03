#include "ProgramInfo.hpp"

ProgramInfo::ProgramInfo(isl::ctx context, const std::string &source) {
  const auto scop = PetUtil::ExtractScop(context, source);
  this->InitStatementInfos(scop);
  PetUtil::FreeScop(scop);
}

const std::map<std::string, StatementInfo> &
ProgramInfo::GetStatementInfos() const {
  return this->statementInfos;
}

void ProgramInfo::InitStatementInfos(pet_scop *scop) {
  this->AddStatementInfosAccessInfos(scop);
  this->AddStatementInfosTimestampInfos(scop);
}

void ProgramInfo::AddStatementInfosAccessInfos(pet_scop *scop) {
  const auto &arrayInfos = this->ExtractArrayInfos(scop);
  const auto &taggedReads = PetUtil::GetTaggedMayReads(scop);
  const auto &taggedWrites = PetUtil::GetTaggedMayWrites(scop);

  this->AddStatementInfosReadAccessInfos(taggedReads, arrayInfos);
  this->AddStatementInfosWriteAccessInfos(taggedWrites, arrayInfos);
  this->SortStatementInfosAccessInfos();
}

void ProgramInfo::AddStatementInfosReadAccessInfos(
    const isl::union_map &taggedReads,
    const std::unordered_map<std::string, ArrayInfo> &arrayInfos) {
  const auto addReadAccessInfo = [&](const isl::map &map) {
    const auto &arrayName = IslUtil::GetMapRangeTupleName(map);
    if (arrayInfos.count(arrayName) == 1) {
      const auto &ai = arrayInfos.at(arrayName);
      const auto &statementName =
          IslUtil::GetMapDomainTupleName(map.domain().unwrap());
      this->statementInfos[statementName].AddReadAccessInfo(ai, map);
    }
  };

  taggedReads.foreach_map(addReadAccessInfo);
}

void ProgramInfo::AddStatementInfosWriteAccessInfos(
    const isl::union_map &taggedWrites,
    const std::unordered_map<std::string, ArrayInfo> &arrayInfos) {
  const auto addWriteAccessInfo = [&](const isl::map &map) {
    const auto &arrayName = IslUtil::GetMapRangeTupleName(map);
    if (arrayInfos.count(arrayName) == 1) {
      const auto &ai = arrayInfos.at(arrayName);
      const auto &statementName =
          IslUtil::GetMapDomainTupleName(map.domain().unwrap());
      this->statementInfos[statementName].AddWriteAccessInfo(ai, map);
    }
  };

  taggedWrites.foreach_map(addWriteAccessInfo);
}

void ProgramInfo::AddStatementInfosTimestampInfos(pet_scop *scop) {
  const auto &timestampToStatement = this->ExtractTimestampToStatement(scop);
  const auto addTimestampInfo = [&](const isl::map &map) {
    const auto &statementName = IslUtil::GetMapRangeTupleName(map);
    assert(this->statementInfos.count(statementName) == 1);
    this->statementInfos.at(statementName).AddTimestampInfo(map);
  };
  timestampToStatement.foreach_map(addTimestampInfo);
}

void ProgramInfo::SortStatementInfosAccessInfos() {
  for (auto &el : this->statementInfos)
    el.second.SortAccessInfos();
}

std::unordered_map<std::string, ArrayInfo>
ProgramInfo::ExtractArrayInfos(pet_scop *scop) const {
  return ArrayInfoExtractor(scop).GetArrayInfos();
}

isl::union_map ProgramInfo::ExtractTimestampToStatement(pet_scop *scop) const {
  const auto &schedule = PetUtil::GetScheduleMap(scop);
  const auto &accessDomain = PetUtil::GetAccessDomain(scop);

  auto statementToTimestamp = isl::union_map::empty(schedule.ctx());
  const auto extendStatementToTimestamp = [&](const isl::map &map) {
    const auto &statementName = IslUtil::GetMapDomainTupleName(map);
    if (this->statementInfos.count(statementName) > 0) {
      statementToTimestamp = statementToTimestamp.unite(map).coalesce();
    }
  };

  schedule.foreach_map(extendStatementToTimestamp);
  statementToTimestamp = statementToTimestamp.intersect_domain(accessDomain);
  return statementToTimestamp.reverse().coalesce();
}

void ProgramInfo::PrintArrayInfos(
    const std::unordered_map<std::string, ArrayInfo> &arrayInfos) const {
  std::cout << std::string(90, '-') << std::endl;
  std::cout << "Array Infos" << std::endl;
  for (const auto &pair : arrayInfos) {
    const auto &ai = pair.second;
    std::cout << std::string(90, '-') << std::endl;
    std::cout << pair.first;
    for (const auto dim : ai.GetDims())
      std::cout << "[" << dim << "]";
    std::cout << std::endl
              << "Element Size: " << ai.GetElementSize() << std::endl
              << "Array Size: " << ai.GetSize() << std::endl
              << "Starting Address: " << ai.GetStartingAddress() << std::endl;
  }
  std::cout << std::endl;
}

void ProgramInfo::PrintStatementInfos() const {
  std::cout << std::endl << std::string(90, '-') << std::endl;
  std::cout << "Statement Infos" << std::endl;
  for (const auto &pair : this->statementInfos) {
    std::cout << std::string(90, '-') << std::endl;
    std::cout << "Name: " << pair.first << std::endl
              << "Ordered Accesses: " << std::endl;
    for (const auto &ai : pair.second.GetAccessInfos()) {
      std::cout << "\t" << (ai.IsRead() ? "Read : " : "Write: ")
                << ai.GetAffineAddress() << std::endl;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
