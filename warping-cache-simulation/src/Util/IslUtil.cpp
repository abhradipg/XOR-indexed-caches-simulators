#include "IslUtil.hpp"

isl_ctx *IslUtil::AllocContext(const std::vector<std::string> &includes) {
  return isl_ctx_alloc_with_options(PetUtil::GetPetOptionsIslArgs(),
                                    PetUtil::GetPetOptions(includes));
}

void IslUtil::FreeContext(isl::ctx context) { isl_ctx_free(context.get()); }

bool IslUtil::IsArrayExtent(const isl::set &extent) {
  return IslUtil::GetSetDimension(extent) > 0;
}

bool IslUtil::IsSetBounded(const isl::set &set) {
  return isl_set_is_bounded(set.get());
}

bool IslUtil::IsAffConstant(const isl::aff &aff) {
  return isl_aff_is_cst(aff.get());
}

bool IslUtil::IsConstraintLowerBound(isl_constraint *constraint, size_t dim) {
  return (isl_constraint_is_lower_bound(constraint, isl_dim_set, dim) ==
          isl_bool_true);
}

bool IslUtil::IsConstraintUpperBound(isl_constraint *constraint, size_t dim) {
  return (isl_constraint_is_upper_bound(constraint, isl_dim_set, dim) ==
          isl_bool_true);
}

isl::aff IslUtil::GetConstraintBound(isl_constraint *constraint, size_t dim) {
  return isl::manage(isl_constraint_get_bound(constraint, isl_dim_set, dim));
}

std::vector<long>
IslUtil::ExtractConstantLowerBounds(const isl::basic_set &bSet, size_t dim) {
  struct ExtractInfo {
    size_t dim = 0;
    std::string dimName;
    std::vector<long> bounds;
  } extractInfo = {dim, IslUtil::GetBasicSetDimName(bSet, dim), {}};

  auto extractor = [](isl_constraint *c, void *extractInfo) {
    auto ei = (ExtractInfo *)extractInfo;
    if (!IslUtil::IsConstraintLowerBound(c, ei->dim)) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }
    assert(IslUtil::GetConstraintDimName(c, ei->dim) == ei->dimName);
    const auto &bound = IslUtil::GetConstraintBound(c, ei->dim);
    if (IslUtil::IsAffConstant(bound))
      ei->bounds.push_back(IslUtil::GetAffConstant(bound));
    isl_constraint_free(c);
    return isl_stat_ok;
  };

  isl_basic_set_foreach_constraint(bSet.get(), extractor,
                                   (void *)(&extractInfo));
  return extractInfo.bounds;
}

std::vector<long>
IslUtil::ExtractConstantUpperBounds(const isl::basic_set &bSet, size_t dim) {
  struct ExtractInfo {
    size_t dim = 0;
    std::string dimName;
    std::vector<long> bounds;
  } extractInfo = {dim, IslUtil::GetBasicSetDimName(bSet, dim), {}};

  auto extractor = [](isl_constraint *c, void *extractInfo) {
    auto ei = (ExtractInfo *)extractInfo;
    if (!IslUtil::IsConstraintUpperBound(c, ei->dim)) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }
    assert(IslUtil::GetConstraintDimName(c, ei->dim) == ei->dimName);
    const auto &bound = IslUtil::GetConstraintBound(c, ei->dim);
    if (IslUtil::IsAffConstant(bound))
      ei->bounds.push_back(IslUtil::GetAffConstant(bound));
    isl_constraint_free(c);
    return isl_stat_ok;
  };

  isl_basic_set_foreach_constraint(bSet.get(), extractor,
                                   (void *)(&extractInfo));
  return extractInfo.bounds;
}

std::vector<isl::aff> IslUtil::ExtractAffLowerBounds(const isl::basic_set &bSet,
                                                     size_t dim) {
  struct ExtractInfo {
    size_t dim = 0;
    std::string dimName;
    std::vector<isl::aff> bounds;
  } extractInfo = {dim, IslUtil::GetBasicSetDimName(bSet, dim), {}};

  auto extractor = [](isl_constraint *c, void *extractInfo) {
    auto ei = (ExtractInfo *)extractInfo;
    if (!IslUtil::IsConstraintLowerBound(c, ei->dim)) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }
    assert(IslUtil::GetConstraintDimName(c, ei->dim) == ei->dimName);
    const auto &bound = IslUtil::GetConstraintBound(c, ei->dim);
    if (!IslUtil::IsAffConstant(bound))
      ei->bounds.push_back(bound);
    isl_constraint_free(c);
    return isl_stat_ok;
  };

  isl_basic_set_foreach_constraint(bSet.get(), extractor,
                                   (void *)(&extractInfo));
  return extractInfo.bounds;
}

std::vector<isl::aff> IslUtil::ExtractAffUpperBounds(const isl::basic_set &bSet,
                                                     size_t dim) {
  struct ExtractInfo {
    size_t dim = 0;
    std::string dimName;
    std::vector<isl::aff> bounds;
  } extractInfo = {dim, IslUtil::GetBasicSetDimName(bSet, dim), {}};

  auto extractor = [](isl_constraint *c, void *extractInfo) {
    auto ei = (ExtractInfo *)extractInfo;
    if (!IslUtil::IsConstraintUpperBound(c, ei->dim)) {
      isl_constraint_free(c);
      return isl_stat_ok;
    }
    assert(IslUtil::GetConstraintDimName(c, ei->dim) == ei->dimName);
    const auto &bound = IslUtil::GetConstraintBound(c, ei->dim);
    if (!IslUtil::IsAffConstant(bound))
      ei->bounds.push_back(bound);
    isl_constraint_free(c);
    return isl_stat_ok;
  };

  isl_basic_set_foreach_constraint(bSet.get(), extractor,
                                   (void *)(&extractInfo));
  return extractInfo.bounds;
}

std::vector<long>
IslUtil::ExtractConstantLowerBounds(const isl::basic_set &bSet,
                                    const std::string &dimName) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::ExtractConstantLowerBounds(bSet, dim);
}

std::vector<long>
IslUtil::ExtractConstantUpperBounds(const isl::basic_set &bSet,
                                    const std::string &dimName) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::ExtractConstantUpperBounds(bSet, dim);
}

std::vector<isl::aff>
IslUtil::ExtractAffLowerBounds(const isl::basic_set &bSet,
                               const std::string &dimName) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::ExtractAffLowerBounds(bSet, dim);
}

std::vector<isl::aff>
IslUtil::ExtractAffUpperBounds(const isl::basic_set &bSet,
                               const std::string &dimName) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::ExtractAffUpperBounds(bSet, dim);
}

std::string IslUtil::GetConstraintDimName(isl_constraint *c, size_t dim) {
  return std::string(isl_constraint_get_dim_name(c, isl_dim_set, dim));
}

size_t IslUtil::GetSetDimension(const isl::set &set) {
  return isl_set_dim(set.get(), isl_dim_set);
}

size_t IslUtil::GetBasicSetDimension(const isl::basic_set &basic_set) {
  return isl_basic_set_dim(basic_set.get(), isl_dim_set);
}

size_t IslUtil::GetPointDimension(const isl::point &point) {
  return IslUtil::GetSetDimension(point);
}

size_t IslUtil::GetMapDomainDimension(const isl::map &map) {
  return isl_space_dim(map.get_space().get(), isl_dim_in);
}

size_t IslUtil::GetMapRangeDimension(const isl::map &map) {
  return isl_space_dim(map.get_space().get(), isl_dim_out);
}

size_t IslUtil::GetAffDomainDimension(const isl::aff &aff) {
  return isl_aff_dim(aff.get(), isl_dim_in);
}

size_t IslUtil::GetPwAffDomainDimension(const isl::pw_aff &pwAff) {
  return isl_pw_aff_dim(pwAff.get(), isl_dim_in);
}

long IslUtil::GetSetDimStride(const isl::set &set, size_t dim) {
  const auto val = set.get_stride(dim);
  assert(val.is_int());
  return val.get_num_si();
}

long IslUtil::GetSetDimNameStride(const isl::set &set,
                                  const std::string &dimName) {
  const auto dim = IslUtil::GetSetDimNamePos(set, dimName);
  return IslUtil::GetSetDimStride(set, dim);
}

long IslUtil::GetPointDimValue(const isl::point &point, size_t dim) {
  const auto val =
      isl::manage(isl_point_get_coordinate_val(point.get(), isl_dim_set, dim));
  assert(val.is_int());
  return val.get_num_si();
}

std::string IslUtil::GetSetTupleName(const isl::set &set) {
  return isl_set_get_tuple_name(set.get());
}

std::string IslUtil::GetMapDomainTupleName(const isl::map &map) {
  return isl_map_get_tuple_name(map.get(), isl_dim_in);
}

std::string IslUtil::GetMapRangeTupleName(const isl::map &map) {
  return isl_map_get_tuple_name(map.get(), isl_dim_out);
}

std::string IslUtil::GetSetDimName(const isl::set &set, size_t pos) {
  return isl_set_get_dim_name(set.get(), isl_dim_set, pos);
}

std::string IslUtil::GetBasicSetDimName(const isl::basic_set &basicSet,
                                        size_t pos) {
  return isl_basic_set_get_dim_name(basicSet.get(), isl_dim_set, pos);
}

std::string IslUtil::GetAffDomainDimName(const isl::aff &aff, size_t pos) {
  return isl_aff_get_dim_name(aff.get(), isl_dim_in, pos);
}

size_t IslUtil::GetBasicSetDimNamePos(const isl::basic_set &basicSet,
                                      const std::string &dimName) {
  auto dim = IslUtil::GetBasicSetDimension(basicSet);
  for (size_t pos = 0; pos < dim; pos++) {
    if (IslUtil::GetBasicSetDimName(basicSet, pos) == dimName)
      return pos;
  }
  assert(false);
  return dim;
}

size_t IslUtil::GetSetDimNamePos(const isl::set &set,
                                 const std::string &dimName) {
  auto dim = IslUtil::GetSetDimension(set);
  for (size_t pos = 0; pos < dim; pos++) {
    assert(isl_set_has_dim_name(set.get(), isl_dim_set, pos));
    if (IslUtil::GetSetDimName(set, pos) == dimName)
      return pos;
  }
  assert(false);
  return dim;
}

long IslUtil::GetAffDomainDimCoefficient(const isl::aff &aff, size_t pos) {
  const auto val =
      isl::manage(isl_aff_get_coefficient_val(aff.get(), isl_dim_in, pos));
  assert(val.is_int());
  return val.get_num_si();
}

long IslUtil::GetAffConstant(const isl::aff &aff) {
  const auto val = isl::manage(isl_aff_get_constant_val(aff.get()));
  assert(val.is_int());
  return val.get_num_si();
}

void IslUtil::AddSetDimensions(isl::set &set, unsigned int nDims) {
  set = isl::manage(isl_set_add_dims(set.get(), isl_dim_set, nDims));
}

void IslUtil::AddMapDomainDimensions(isl::map &map, unsigned int nDims) {
  map = isl::manage(isl_map_add_dims(map.get(), isl_dim_in, nDims));
}

void IslUtil::AddMapRangeDimensions(isl::map &map, unsigned int nDims) {
  map = isl::manage(isl_map_add_dims(map.get(), isl_dim_out, nDims));
}

std::unordered_map<std::string, std::string>
IslUtil::ExtractSymbolicNameMapping(const isl::map &map) {
  std::unordered_map<std::string, std::string> affineExprNameEquivalents;
  const auto &dimStrs = IslUtil::ConvertMapDomainTupleToStringVector(map);
  const auto isValidDimStr = [](const std::string &dimStr) {
    std::regex validNameRegex("^-?i[0123456789]+$");
    return std::regex_match(dimStr, validNameRegex);
  };
  for (size_t i = 0; i < dimStrs.size(); i++) {
    const auto &dimStr = dimStrs[i];
    if (StrUtil::IsInteger(dimStrs[i]))
      continue;

    const auto &iStr = std::to_string(i);
    if (isValidDimStr(dimStr)) {
      assert(dimStr == "i" + iStr || dimStr == "-i" + iStr);
      continue;
    }

    assert(affineExprNameEquivalents.count(dimStr) == 0);
    affineExprNameEquivalents.emplace(dimStr, "i" + iStr);
  }

  std::unordered_map<std::string, std::string> symbolicNameMapping;
  auto mapStr = IslUtil::ToString(map);
  auto afterArrow = mapStr.find("->") + 2;
  auto colon = mapStr.find(':', afterArrow + 2);
  auto statementStr = mapStr.substr(afterArrow, colon - afterArrow);
  auto first = statementStr.find('[');
  auto second = statementStr.find(']');
  auto target = statementStr.substr(first + 1, second - first - 1);
  auto end_pos = std::remove(target.begin(), target.end(), ' ');
  target.erase(end_pos, target.end());
  const auto &namePairs = StrUtil::SplitStringByChar(target, ',');
  for (const auto &namePair : namePairs) {
    const auto &pair = StrUtil::SplitStringByChar(namePair, '=');
    const auto &oldName = pair[0];
    std::string newName = pair[1];
    if (!isValidDimStr(newName)) {
      assert(affineExprNameEquivalents.count(newName) == 1);
      newName = affineExprNameEquivalents[newName];
    }
    assert(symbolicNameMapping.count(oldName) == 0);
    symbolicNameMapping[oldName] = newName;
  }
  return symbolicNameMapping;
}

std::vector<isl::aff>
IslUtil::ExtractAffineIndices(const isl::map &taggedReadOrWrite) {
  // TODO: simplify
  std::vector<isl::aff> affineIndices;
  const auto &stmtName =
      IslUtil::GetMapDomainTupleName(taggedReadOrWrite.domain().unwrap());
  auto affLeft = "{ " + stmtName + "[";
  const auto leftDomain = taggedReadOrWrite.domain().unwrap().domain();
  const auto nDims = IslUtil::GetSetDimension(leftDomain);
  for (size_t pos = 0; pos < nDims; pos++) {
    affLeft += IslUtil::GetSetDimName(leftDomain, pos);
    if (pos != nDims - 1)
      affLeft += ",";
  }
  affLeft += "] -> ";

  const auto &str = IslUtil::ToString(taggedReadOrWrite);
  const auto &arrName = IslUtil::GetMapRangeTupleName(taggedReadOrWrite);
  auto firstPos = str.find('{');
  size_t arrNameStart = 0;
  arrNameStart = str.find("->", firstPos) + 2;
  assert(arrNameStart != 0 && arrNameStart != std::string::npos);
  auto secondArrowPos = str.find("->", arrNameStart) + 2;
  if (secondArrowPos != std::string::npos)
    arrNameStart = secondArrowPos;
  auto arrNamePos = str.find(arrName, arrNameStart);
  auto out1 = str.find('[', arrNamePos + arrName.length());
  auto out2 = str.find(']', out1);
  auto outTarget = str.substr(out1 + 1, out2 - out1 - 1);
  auto affOuts = StrUtil::SplitStringByChar(outTarget, ',');
  for (const auto &affOut : affOuts) {
    auto affRight = " [ " + affOut + " ] }";
    auto aff = isl::aff(taggedReadOrWrite.ctx(), affLeft + affRight);
    affineIndices.push_back(aff);
  }
  return affineIndices;
}

isl::set IslUtil::EliminateDimsAfterDim(const isl::set &set, size_t dim) {
  auto copy = isl::manage_copy(set.get());
  auto nDims = IslUtil::GetSetDimension(copy);
  auto newSet = isl::manage(
      isl_set_eliminate_dims(copy.release(), dim + 1, nDims - dim - 1));
  // create from string
  // otherwise dimensions do not have names in IteratorState::InitBounds
  // TODO: fix this, probably related to timestamp passed to IteratorState
  return isl::set(newSet.ctx(), IslUtil::ToString(newSet));
}

isl::set IslUtil::DropDimsAfterDim(const isl::set &set, size_t dim) {
  auto str = IslUtil::ToString(set);
  auto colon = str.find(':');
  assert(colon != std::string::npos);
  const auto rightStr = str.substr(colon);
  auto dimNames = IslUtil::ConvertSetTupleToStringVector(set);
  assert(!dimNames.empty());
  std::string leftStr = "{[";
  for (size_t pos = 0; pos <= dim; pos++) {
    if (StrUtil::IsInteger(dimNames[pos]))
      continue;
    leftStr.append(dimNames[pos]);
    if (pos != dim)
      leftStr.append(",");
  }
  leftStr.append("] ");
  return isl::set(set.ctx(), leftStr + rightStr);
}

std::vector<std::string> IslUtil::GetSetDimNames(const isl::set &set) {
  std::vector<std::string> names;
  auto nDims = IslUtil::GetSetDimension(set);
  for (size_t dim = 0; dim < nDims; dim++)
    names.emplace_back(isl_set_get_dim_name(set.get(), isl_dim_set, dim));
  return names;
}

std::vector<std::string> IslUtil::GetAffDimNames(const isl::aff &aff) {
  std::vector<std::string> names;
  auto nDims = IslUtil::GetAffDomainDimension(aff);
  for (size_t dim = 0; dim < nDims; dim++)
    names.emplace_back(isl_aff_get_dim_name(aff.get(), isl_dim_in, dim));
  return names;
}

std::vector<std::string> IslUtil::GetPwAffDimNames(const isl::pw_aff &pwAff) {
  std::vector<std::string> names;
  auto nDims = IslUtil::GetPwAffDomainDimension(pwAff);
  for (size_t dim = 0; dim < nDims; dim++)
    names.emplace_back(isl_pw_aff_get_dim_name(pwAff.get(), isl_dim_in, dim));
  return names;
}

isl::set IslUtil::RenameSetDims(
    const isl::set &set,
    const std::unordered_map<std::string, std::string> &oldToNewNames) {
  auto setCopy = set.copy();
  auto nDims = IslUtil::GetSetDimension(set);
  auto hadMinus = false;
  for (size_t dim = 0; dim < nDims; dim++) {
    assert(isl_set_has_dim_name(setCopy, isl_dim_set, dim));
    std::string oldName = isl_set_get_dim_name(setCopy, isl_dim_set, dim);
    auto it = oldToNewNames.find(oldName);
    assert(it != oldToNewNames.end());
    auto newName = it->second;
    if (newName[0] == '-')
      hadMinus = true;
    setCopy = isl_set_set_dim_name(setCopy, isl_dim_set, dim, newName.c_str());
  }

  auto res = isl::manage(setCopy);
  if (hadMinus) {
    auto str = IslUtil::ToString(res);
    auto col = str.find(':');
    auto left = str.substr(0, col);
    auto right = str.substr(col);
    left = StrUtil::ReplaceAllOccurrences(left, "-i", "i");
    auto newStr = left + right;
    res = isl::set(set.ctx(), newStr);
  }
  return res;
}

isl::aff IslUtil::RenameAffDims(
    const isl::aff &aff,
    const std::unordered_map<std::string, std::string> &oldToNewNames) {
  auto affCopy = aff.copy();
  auto nDims = IslUtil::GetAffDomainDimension(aff);
  for (size_t dim = 0; dim < nDims; dim++) {
    std::string oldName = isl_aff_get_dim_name(affCopy, isl_dim_in, dim);
    auto it = oldToNewNames.find(oldName);
    assert(it != oldToNewNames.end());
    auto newName = it->second;
    if (newName[0] == '-') {
      newName = newName.substr(1);
      auto coefficient =
          isl::manage(isl_aff_get_coefficient_val(affCopy, isl_dim_in, dim))
              .get_num_si();
      auto newCoefficient = -1 * coefficient;
      affCopy =
          isl_aff_set_coefficient_si(affCopy, isl_dim_in, dim, newCoefficient);
    }
    affCopy = isl_aff_set_dim_name(affCopy, isl_dim_in, dim, newName.c_str());
  }
  return isl::manage(affCopy);
}

isl::set IslUtil::AppendToSetDimNames(const isl::set &set,
                                      const std::string &str) {
  auto setCopy = set.copy();
  auto nDims = IslUtil::GetSetDimension(set);
  for (size_t dim = 0; dim < nDims; dim++) {
    const auto &oldName = isl_set_get_dim_name(setCopy, isl_dim_set, dim);
    auto newName = oldName + str;
    setCopy = isl_set_set_dim_name(setCopy, isl_dim_set, dim, newName.c_str());
  }
  return isl::manage(setCopy);
}

isl::aff IslUtil::AppendToAffDimNames(const isl::aff &aff,
                                      const std::string &str) {
  auto affCopy = aff.copy();
  auto nDims = IslUtil::GetAffDomainDimension(aff);
  for (size_t dim = 0; dim < nDims; dim++) {
    const auto &oldName = isl_aff_get_dim_name(affCopy, isl_dim_in, dim);
    auto newName = oldName + str;
    affCopy = isl_aff_set_dim_name(affCopy, isl_dim_in, dim, newName.c_str());
  }
  return isl::manage(affCopy);
}

std::string IslUtil::GetPwAffRangeString(const isl::pw_aff &pwAff) {
  const auto &str = IslUtil::ToString(pwAff);
  auto initPos = str.find('{');
  auto arrowPos = str.find("->", initPos);
  auto start = str.find('[', arrowPos) + 1;
  auto end = str.find(']', start) - 1;
  auto len = end - start + 1;
  return str.substr(start, len);
}

std::string IslUtil::GetPwAffConditionsString(const isl::pw_aff &pwAff) {
  const auto &str = IslUtil::ToString(pwAff);
  auto initPos = str.find('{');
  auto colonPos = str.find(':', initPos);
  if (colonPos == std::string::npos)
    return "";
  auto start = colonPos + 1;
  auto end = str.find('}', colonPos) - 1;
  auto len = end - start + 1;
  return str.substr(start, len);
}

std::string IslUtil::CreateConflictSetStringBasis(
    const isl::aff &aff1, const isl::aff &aff2, const isl::set &dom1,
    const isl::set &dom2, const std::string &appendStr1,
    const std::string &appendStr2) {

  auto a1 = IslUtil::AppendToAffDimNames(aff1, appendStr1);
  auto d1 = IslUtil::AppendToSetDimNames(dom1, appendStr1);
  auto pw1 = isl::pw_aff(a1).intersect_domain(d1);
  const auto &names1 = IslUtil::GetPwAffDimNames(pw1);
  assert(!names1.empty());

  auto a2 = IslUtil::AppendToAffDimNames(aff2, appendStr2);
  auto d2 = IslUtil::AppendToSetDimNames(dom2, appendStr2);
  auto pw2 = isl::pw_aff(a2).intersect_domain(d2);
  const auto &names2 = IslUtil::GetPwAffDimNames(pw2);
  assert(!names2.empty());

  std::string conflictSetStr = "{ [x] ";
  for (const auto &name : names1)
    conflictSetStr.append(" : exists " + name);
  for (const auto &name : names2)
    conflictSetStr.append(" : exists " + name);
  conflictSetStr.append(" : ");

  const auto &funcStr1 = StrUtil::Format(
      "floor(%s / %d)", IslUtil::GetPwAffRangeString(pw1).c_str(),
      GlobalVars::lineSize);
  const auto &funcStr2 = StrUtil::Format(
      "floor(%s / %d)", IslUtil::GetPwAffRangeString(pw2).c_str(),
      GlobalVars::lineSize);
  conflictSetStr.append(" " + funcStr1 + " = " + funcStr2 + " ");

  const auto &condStr1 = IslUtil::GetPwAffConditionsString(pw1);
  const auto &condStr2 = IslUtil::GetPwAffConditionsString(pw2);
  if (!condStr1.empty())
    conflictSetStr.append(" and " + condStr1);
  if (!condStr2.empty())
    conflictSetStr.append(" and " + condStr2);

  return conflictSetStr;
}

std::string IslUtil::GetConditionString(const isl::set &set) {
  const auto &str = IslUtil::ToString(set);
  auto initPos = str.find('{');
  auto colonPos = str.find(':', initPos);
  assert(colonPos != std::string::npos);
  auto start = colonPos + 1;
  auto end = str.find('}', colonPos) - 1;
  auto len = end - start + 1;
  return str.substr(start, len);
}

bool IslUtil::IsVarInConditionString(const std::string &var,
                                     const std::string &str) {
  // TODO: regex matches if have something like asdf{$var}
  // we assume such it names cannot exist..
  std::regex reg(var + "(?![a-zA-Z0-9_])");
  return std::regex_search(str, reg);
}

isl::point IslUtil::CreateSingleDimPoint(isl::ctx ctx, long val) {
  auto v = isl::val(ctx, val);
  auto s = isl_space_set_alloc(ctx.get(), 0, 1);
  auto p = isl_point_zero(s);
  p = isl_point_set_coordinate_val(p, isl_dim_set, 0, v.release());
  return isl::manage(p);
}

isl::point IslUtil::CreateSingleDimPoint(isl::ctx ctx, long val,
                                         const isl::space &space) {
  return IslUtil::CreateSingleDimPoint(ctx, val, space, {});
}

isl::point IslUtil::CreateSingleDimPoint(isl::ctx ctx, long val,
                                         const isl::space &space,
                                         const std::vector<long> &params) {
  auto v = isl::val(ctx, val);
  auto p = isl_point_zero(space.copy());
  p = isl_point_set_coordinate_val(p, isl_dim_set, 0, v.release());
  for (size_t i = 0; i < params.size(); i++) {
    auto pv = isl::val(ctx, params[i]);
    p = isl_point_set_coordinate_val(p, isl_dim_param, i, pv.release());
  }
  return isl::manage(p);
}

isl::set IslUtil::CreateEmptySet(const isl::space &space) {
  auto emptySet = isl::manage(isl_set_empty(space.copy()));
  assert(emptySet.is_empty());
  return emptySet;
}

isl::basic_set
IslUtil::CreateBasicSetFromVal(isl::ctx ctx, long val, const isl::space &space,
                               const std::vector<long> &paramVals) {
  auto point = IslUtil::CreateSingleDimPoint(ctx, val, space, paramVals);
  return isl::manage(isl_basic_set_from_point(point.release()));
}

isl::multi_aff
IslUtil::GetBasicMapDomainMultiAffIdentity(const isl::basic_map &bMap) {
  const auto &dom = IslUtil::GetBasicMapDomain(bMap);
  return IslUtil::GetBasicSetMultiAffIdentity(dom);
}

isl::multi_aff
IslUtil::GetBasicSetMultiAffIdentity(const isl::basic_set &bSet) {
  auto space = IslUtil::GetBasicSetSpace(bSet).release();
  return isl::manage(isl_multi_aff_identity_on_domain_space(space));
}

isl::aff IslUtil::GetAffValOnBasicSetSpace(long val,
                                           const isl::basic_set &bSet) {
  auto valP = isl_val_int_from_si(bSet.ctx().get(), val);
  auto space = IslUtil::GetBasicSetSpace(bSet);
  return isl::manage(isl_aff_val_on_domain_space(space.copy(), valP));
}

isl::basic_set IslUtil::AddBasicSetEqConstraint(const isl::basic_set &bSet,
                                                const isl::multi_aff &mAff,
                                                size_t dim, long val) {
  auto var = mAff.at(dim).release();
  auto valAff = IslUtil::GetAffValOnBasicSetSpace(val, bSet);
  auto condBSet = isl::manage(isl_aff_eq_basic_set(var, valAff.release()));
  return bSet.intersect(condBSet);
}

isl::basic_set
IslUtil::AddBasicSetGreaterEqConstraint(const isl::basic_set &bSet,
                                        const isl::multi_aff &mAff, size_t dim,
                                        long val) {
  auto var = mAff.at(dim).release();
  auto valAff = IslUtil::GetAffValOnBasicSetSpace(val, bSet);
  auto condBSet = isl::manage(isl_aff_ge_basic_set(var, valAff.release()));
  return bSet.intersect(condBSet);
}

isl::basic_set IslUtil::AddBasicSetLessEqConstraint(const isl::basic_set &bSet,
                                                    const isl::multi_aff &mAff,
                                                    size_t dim, long val) {
  auto var = mAff.at(dim).release();
  auto valAff = IslUtil::GetAffValOnBasicSetSpace(val, bSet);
  auto condBSet = isl::manage(isl_aff_le_basic_set(var, valAff.release()));
  return bSet.intersect(condBSet);
}

isl::basic_set IslUtil::AddBasicSetGreaterConstraint(const isl::basic_set &bSet,
                                                     const isl::multi_aff &mAff,
                                                     size_t dim, long val) {
  auto var = mAff.at(dim).release();
  auto valAff = IslUtil::GetAffValOnBasicSetSpace(val, bSet);
  auto condBSet = isl::manage(isl_aff_gt_basic_set(var, valAff.release()));
  return bSet.intersect(condBSet);
}

isl::basic_set IslUtil::AddBasicSetLessConstraint(const isl::basic_set &bSet,
                                                  const isl::multi_aff &mAff,
                                                  size_t dim, long val) {
  auto var = mAff.at(dim).release();
  auto valAff = IslUtil::GetAffValOnBasicSetSpace(val, bSet);
  auto condBSet = isl::manage(isl_aff_lt_basic_set(var, valAff.release()));
  return bSet.intersect(condBSet);
}

isl::basic_set IslUtil::AddBasicSetEqConstraint(const isl::basic_set &bSet,
                                                const isl::multi_aff &mAff,
                                                const std::string &dimName,
                                                long val) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::AddBasicSetEqConstraint(bSet, mAff, dim, val);
}

isl::basic_set
IslUtil::AddBasicSetGreaterEqConstraint(const isl::basic_set &bSet,
                                        const isl::multi_aff &mAff,
                                        const std::string &dimName, long val) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::AddBasicSetGreaterEqConstraint(bSet, mAff, dim, val);
}

isl::basic_set IslUtil::AddBasicSetLessEqConstraint(const isl::basic_set &bSet,
                                                    const isl::multi_aff &mAff,
                                                    const std::string &dimName,
                                                    long val) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::AddBasicSetLessEqConstraint(bSet, mAff, dim, val);
}

isl::basic_set IslUtil::AddBasicSetGreaterConstraint(const isl::basic_set &bSet,
                                                     const isl::multi_aff &mAff,
                                                     const std::string &dimName,
                                                     long val) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::AddBasicSetGreaterConstraint(bSet, mAff, dim, val);
}

isl::basic_set IslUtil::AddBasicSetLessConstraint(const isl::basic_set &bSet,
                                                  const isl::multi_aff &mAff,
                                                  const std::string &dimName,
                                                  long val) {
  const auto dim = IslUtil::GetBasicSetDimNamePos(bSet, dimName);
  return IslUtil::AddBasicSetLessConstraint(bSet, mAff, dim, val);
}

isl::basic_map
IslUtil::AddBasicMapDomainEqConstraint(const isl::basic_map &bMap,
                                       const isl::multi_aff &mAff, size_t dim,
                                       long val) {
  const auto &condBSet = IslUtil::AddBasicSetEqConstraint(
      IslUtil::GetBasicMapDomain(bMap), mAff, dim, val);
  return bMap.intersect_domain(condBSet);
}

isl::basic_map
IslUtil::AddBasicMapDomainGreaterEqConstraint(const isl::basic_map &bMap,
                                              const isl::multi_aff &mAff,
                                              size_t dim, long val) {
  const auto &condBSet = IslUtil::AddBasicSetGreaterEqConstraint(
      IslUtil::GetBasicMapDomain(bMap), mAff, dim, val);
  return bMap.intersect_domain(condBSet);
}

isl::basic_map
IslUtil::AddBasicMapDomainLessEqConstraint(const isl::basic_map &bMap,
                                           const isl::multi_aff &mAff,
                                           size_t dim, long val) {
  const auto &condBSet = IslUtil::AddBasicSetLessEqConstraint(
      IslUtil::GetBasicMapDomain(bMap), mAff, dim, val);
  return bMap.intersect_domain(condBSet);
}

isl::basic_map
IslUtil::AddBasicMapDomainGreaterConstraint(const isl::basic_map &bMap,
                                            const isl::multi_aff &mAff,
                                            size_t dim, long val) {
  const auto &condBSet = IslUtil::AddBasicSetGreaterConstraint(
      IslUtil::GetBasicMapDomain(bMap), mAff, dim, val);
  return bMap.intersect_domain(condBSet);
}

isl::basic_map
IslUtil::AddBasicMapDomainLessConstraint(const isl::basic_map &bMap,
                                         const isl::multi_aff &mAff, size_t dim,
                                         long val) {
  const auto &condBSet = IslUtil::AddBasicSetLessConstraint(
      IslUtil::GetBasicMapDomain(bMap), mAff, dim, val);
  return bMap.intersect_domain(condBSet);
}

isl::basic_map
IslUtil::AddBasicMapDomainEqConstraint(const isl::basic_map &bMap,
                                       const isl::multi_aff &mAff,
                                       const std::string &dimName, long val) {
  const auto dim =
      IslUtil::GetBasicSetDimNamePos(IslUtil::GetBasicMapDomain(bMap), dimName);
  return IslUtil::AddBasicMapDomainEqConstraint(bMap, mAff, dim, val);
}

isl::basic_map IslUtil::AddBasicMapDomainGreaterEqConstraint(
    const isl::basic_map &bMap, const isl::multi_aff &mAff,
    const std::string &dimName, long val) {
  const auto dim =
      IslUtil::GetBasicSetDimNamePos(IslUtil::GetBasicMapDomain(bMap), dimName);
  return IslUtil::AddBasicMapDomainGreaterEqConstraint(bMap, mAff, dim, val);
}

isl::basic_map IslUtil::AddBasicMapDomainLessEqConstraint(
    const isl::basic_map &bMap, const isl::multi_aff &mAff,
    const std::string &dimName, long val) {
  const auto dim =
      IslUtil::GetBasicSetDimNamePos(IslUtil::GetBasicMapDomain(bMap), dimName);
  return IslUtil::AddBasicMapDomainLessEqConstraint(bMap, mAff, dim, val);
}

isl::basic_map IslUtil::AddBasicMapDomainGreaterConstraint(
    const isl::basic_map &bMap, const isl::multi_aff &mAff,
    const std::string &dimName, long val) {
  const auto dim =
      IslUtil::GetBasicSetDimNamePos(IslUtil::GetBasicMapDomain(bMap), dimName);
  return IslUtil::AddBasicMapDomainGreaterConstraint(bMap, mAff, dim, val);
}

isl::basic_map
IslUtil::AddBasicMapDomainLessConstraint(const isl::basic_map &bMap,
                                         const isl::multi_aff &mAff,
                                         const std::string &dimName, long val) {
  const auto dim =
      IslUtil::GetBasicSetDimNamePos(IslUtil::GetBasicMapDomain(bMap), dimName);
  return IslUtil::AddBasicMapDomainLessConstraint(bMap, mAff, dim, val);
}

isl::basic_set IslUtil::EvalBasicMap(const isl::basic_map &bMap, long val,
                                     const std::vector<long> &paramVals) {
  const auto &bSet = IslUtil::CreateBasicSetFromVal(
      bMap.ctx(), val, IslUtil::GetBasicMapDomainSpace(bMap), paramVals);
  return bSet.apply(bMap);
}

isl::basic_set
IslUtil::SetBasicSetParamValues(const isl::basic_set &bSet,
                                const std::vector<long> &paramValues) {
  auto b = bSet.copy();
  for (size_t i = 0; i < paramValues.size(); i++)
    b = isl_basic_set_fix_si(b, isl_dim_param, i, paramValues[i]);
  return isl::manage(b);
}

isl::basic_set IslUtil::GetBasicMapDomain(const isl::basic_map &bMap) {
  return isl::manage(isl_basic_map_domain(bMap.copy()));
}

isl::basic_set IslUtil::GetBasicMapRange(const isl::basic_map &bMap) {
  return isl::manage(isl_basic_map_range(bMap.copy()));
}

isl::space IslUtil::GetBasicMapDomainSpace(const isl::basic_map &bMap) {
  return IslUtil::GetBasicSetSpace(IslUtil::GetBasicMapDomain(bMap));
}

isl::space IslUtil::GetBasicSetSpace(const isl::basic_set &bSet) {
  return isl::manage(isl_basic_set_get_space(bSet.get()));
}

isl::basic_map IslUtil::UnwrapBasicSet(const isl::basic_set &bSet) {
  assert(bSet.is_wrapping());
  return isl::manage(isl_basic_set_unwrap(bSet.copy()));
}

isl::basic_map IslUtil::DivAndFloorBasicMap(const isl::basic_map &bMap,
                                            long divisor) {
  assert(divisor != 0);
  constexpr auto floorRelFormat = "{[x] -> [floor(x / %d)]}";
  auto floorRelStr = StrUtil::Format(floorRelFormat, divisor);
  auto floorRel = isl::basic_map(bMap.ctx(), floorRelStr);
  return bMap.apply_range(floorRel);
}

isl::basic_map IslUtil::ConvertPwAffToBasicMap(const isl::pw_aff &pwAff) {
  const auto &pwAffStr = IslUtil::ToString(pwAff);
  return isl::basic_map(pwAff.ctx(), pwAffStr);
}

isl::pw_aff IslUtil::ConvertAffToPwAff(const isl::aff &aff) {
  return isl::manage(isl_pw_aff_from_aff(aff.copy()));
}

isl::pw_aff IslUtil::ConvertAffToPwAff(const isl::aff &aff,
                                       const isl::set &domain) {
  return IslUtil::ConvertAffToPwAff(aff).intersect_domain(domain);
}

isl::set IslUtil::RemoveSetTupleName(const isl::set &set) {
  auto str = IslUtil::ToString(set);
  auto startPos = str.find('{');
  auto newStartPos = str.find('[', startPos);
  auto newStr = "{ " + str.substr(newStartPos);
  return isl::set(set.ctx(), newStr);
}

isl::aff IslUtil::RemoveAffTupleName(const isl::aff &aff) {
  auto str = IslUtil::ToString(aff);
  auto startPos = str.find('{');
  auto newStartPos = str.find('[', startPos);
  auto newStr = "{ " + str.substr(newStartPos);
  return isl::aff(aff.ctx(), newStr);
}

isl::pw_aff IslUtil::RemovePwAffTupleName(const isl::pw_aff &pwAff) {
  auto str = IslUtil::ToString(pwAff);
  auto startPos = str.find('{');
  auto newStartPos = str.find('[', startPos);
  auto newStr = "{ " + str.substr(newStartPos);
  return isl::pw_aff(pwAff.ctx(), newStr);
}

std::string IslUtil::AddIslStringCondition(const std::string &str,
                                           const std::string &condition) {
  auto pos = str.find('}');
  assert(pos != std::string::npos);
  auto cond = str.find(':') == std::string::npos ? " : " + condition + " "
                                                 : " and " + condition + " ";
  auto newStr = str;
  return newStr.insert(pos, cond);
}

isl::pw_aff IslUtil::RemoveAllConditions(const isl::pw_aff &pwAff) {
  auto str = IslUtil::ToString(pwAff);
  auto colonPos = str.find(':');
  assert(colonPos != std::string::npos);
  auto newStr = str.substr(0, colonPos).append("}");
  return isl::pw_aff(pwAff.ctx(), newStr);
}

isl::pw_aff IslUtil::AddPwAffEqualCondition(const isl::pw_aff &pwAff,
                                            const std::string &var, long val) {
  const auto &cond = var + " = " + std::to_string(val);
  return IslUtil::AddPwAffCondition(pwAff, cond);
}

isl::pw_aff IslUtil::AddPwAffCondition(const isl::pw_aff &pwAff,
                                       const std::string &condition) {
  const auto &str = IslUtil::ToString(pwAff);
  const auto &newStr = IslUtil::AddIslStringCondition(str, condition);
  return isl::pw_aff(pwAff.ctx(), newStr);
}

isl::set IslUtil::AddSetEqualCondition(const isl::set &set,
                                       const std::string &var, long val) {
  const auto &cond = var + " = " + std::to_string(val);
  return IslUtil::AddSetCondition(set, cond);
}

isl::set IslUtil::AddSetCondition(const isl::set &set,
                                  const std::string &condition) {
  const auto &str = IslUtil::ToString(set);
  const auto &newStr = IslUtil::AddIslStringCondition(str, condition);
  return isl::set(set.ctx(), newStr);
}

isl::basic_set
IslUtil::AddBasicSetEqualCondition(const isl::basic_set &basicSet,
                                   const std::string &var, long val) {
  const auto &cond = var + " = " + std::to_string(val);
  return IslUtil::AddBasicSetCondition(basicSet, cond);
}

isl::basic_set IslUtil::AddBasicSetGreaterThanOrEqualToCondition(
    const isl::basic_set &basicSet, const std::string &var, long val) {
  const auto &cond = var + " >= " + std::to_string(val);
  return IslUtil::AddBasicSetCondition(basicSet, cond);
}

isl::basic_set IslUtil::AddBasicSetLessThanOrEqualToCondition(
    const isl::basic_set &basicSet, const std::string &var, long val) {
  const auto &cond = var + " <= " + std::to_string(val);
  return IslUtil::AddBasicSetCondition(basicSet, cond);
}

isl::basic_set IslUtil::AddBasicSetCondition(const isl::basic_set &basicSet,
                                             const std::string &condition) {
  const auto &str = IslUtil::ToString(basicSet);
  const auto &newStr = IslUtil::AddIslStringCondition(str, condition);
  return isl::basic_set(basicSet.ctx(), newStr);
}

isl::set IslUtil::ConvertUnionSetToSet(isl::union_set uset) {
  assert(uset.isa_set());
  return isl::manage(isl_set_from_union_set(uset.release()));
}

isl::map IslUtil::ConvertUnionMapToMap(isl::union_map umap) {
  assert(umap.isa_map());
  return isl::manage(isl_map_from_union_map(umap.release()));
}

isl::basic_map IslUtil::ConvertMapToBasicMap(const isl::map &map) {
  return map.polyhedral_hull();
}

std::vector<long> IslUtil::ConvertPointToVector(const isl::point &point) {
  std::vector<long> vec;
  size_t nDims = IslUtil::GetPointDimension(point);
  for (size_t i = 0; i < nDims; i++) {
    const auto val =
        isl::manage(isl_point_get_coordinate_val(point.get(), isl_dim_set, i));
    assert(val.is_int());
    vec.push_back(val.num_si());
  }
  return vec;
}

std::vector<std::string>
IslUtil::ConvertSetTupleToStringVector(const isl::set &set) {
  auto str = IslUtil::ToString(set);
  auto start = str.find('{');
  auto first = str.find('[', start);
  auto second = str.find(']', first);
  str = str.substr(first + 1, second - first - 1);
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  return StrUtil::SplitStringByChar(str, ',');
}

std::vector<std::string>
IslUtil::ConvertBasicSetTupleToStringVector(const isl::basic_set &bSet) {
  // TODO: encapsulate duplicate code..
  auto str = IslUtil::ToString(bSet);
  auto start = str.find('{');
  auto first = str.find('[', start);
  auto second = str.find(']', first);
  str = str.substr(first + 1, second - first - 1);
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  return StrUtil::SplitStringByChar(str, ',');
}

std::vector<std::string>
IslUtil::ConvertMapDomainTupleToStringVector(const isl::map &map) {
  auto str = IslUtil::ToString(map);
  auto start = str.find('{');
  auto first = str.find('[', start);
  auto second = str.find(']', first);
  str = str.substr(first + 1, second - first - 1);
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  return StrUtil::SplitStringByChar(str, ',');
}

isl::basic_set IslUtil::NegateBasicSet(const isl::basic_set &basicSet) {
  return isl::manage(isl_basic_set_neg(basicSet.copy()));
}

isl::basic_set IslUtil::ConvertSetToBasicSet(const isl::set &set) {
  assert(!set.is_null());
  const auto &bSet = isl::basic_set(set.ctx(), IslUtil::ToString(set));
  // assert(bSet.is_equal(set.polyhedral_hull()));
  return bSet;
}

long IslUtil::GetBasicSetDimMaxValue(const isl::basic_set &basicSet,
                                     size_t pos) {
  return IslUtil::ToLong(basicSet.dim_max_val(pos));
}

long IslUtil::GetBasicSetDimMinValue(const isl::basic_set &basicSet,
                                     size_t pos) {
  auto negatedBasicSet = IslUtil::NegateBasicSet(basicSet);
  return -1 * IslUtil::GetBasicSetDimMaxValue(negatedBasicSet, pos);
}

bool IslUtil::IsBasicSetDimMaxValBounded(const isl::basic_set &bSet,
                                         size_t dim) {
  auto val = isl::manage(isl_basic_set_dim_max_val(bSet.copy(), dim));
  assert(!val.is_null());
  return !val.is_nan() && !val.is_infty() && val.is_int();
}

long IslUtil::ToLong(const isl::val &val) { return val.get_num_si(); }

std::string IslUtil::ConstructIffConditionString(const std::string &cond1,
                                                 const std::string &cond2) {
  auto left = "((" + cond1 + ") implies (" + cond2 + "))";
  auto right = "((" + cond2 + ") implies (" + cond1 + "))";
  return "(" + left + ") and (" + right + ")";
}

std::string IslUtil::NegateConditionString(const std::string &cond) {
  return "(!(" + cond + "))";
}

std::string
IslUtil::ReplaceIteratorWithExpressionString(const std::string &itStr,
                                             const std::string &exprStr,
                                             const std::string &condStr) {
  std::regex reg(itStr + "(?![a-zA-Z0-9_])");
  return std::regex_replace(condStr, reg, exprStr);
}

std::string IslUtil::GetStringWithoutBraces(const isl::set &set) {
  const auto &str = IslUtil::ToString(set);
  auto openBracePos = str.find('{');
  auto closeBracePos = str.find('}');
  assert(openBracePos == 0);
  assert(closeBracePos == str.length() - 1);
  const auto &res = str.substr(openBracePos + 1, closeBracePos - 1);
  assert(!res.empty());
  assert(res.front() != '{');
  assert(res.back() != '}');
  return res;
}

// TODO: implement single ToString using a generic class or a parent class?
std::string IslUtil::ToString(const isl::point &point) {
  std::ostringstream oss;
  oss << point;
  return oss.str();
}

std::string IslUtil::ToString(const isl::space &space) {
  std::ostringstream oss;
  oss << space;
  return oss.str();
}

std::string IslUtil::ToString(const isl::set &set) {
  std::ostringstream oss;
  oss << set;
  return oss.str();
}

std::string IslUtil::ToString(const isl::basic_set &bSet) {
  std::ostringstream oss;
  oss << bSet;
  return oss.str();
}

std::string IslUtil::ToString(const isl::map &map) {
  std::ostringstream oss;
  oss << map;
  return oss.str();
}

std::string IslUtil::ToString(const isl::aff &aff) {
  std::ostringstream oss;
  oss << aff;
  return oss.str();
}

std::string IslUtil::ToString(const isl::pw_aff &pwAff) {
  std::ostringstream oss;
  oss << pwAff;
  return oss.str();
}
