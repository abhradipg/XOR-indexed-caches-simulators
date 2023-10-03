#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <isl/aff.h>
#include <isl/constraint.h>
#include <isl/cpp.h>
#include <isl/point.h>
#include <isl/schedule.h>
#include <list>
#include <map>
#include <pet.h>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "PetUtil.hpp"
#include "StrUtil.hpp"
#include "Util.hpp"

namespace IslUtil {

isl_ctx *AllocContext(const std::vector<std::string> &includes);
void FreeContext(isl::ctx context);

bool IsArrayExtent(const isl::set &extent);

bool IsSetBounded(const isl::set &set);

bool IsAffConstant(const isl::aff &aff);
bool IsConstraintLowerBound(isl_constraint *constraint, size_t dim);
bool IsConstraintUpperBound(isl_constraint *constraint, size_t dim);
isl::aff GetConstraintBound(isl_constraint *constraint, size_t dim);

std::vector<long> ExtractConstantLowerBounds(const isl::basic_set &bSet,
                                             size_t dim);
std::vector<long> ExtractConstantUpperBounds(const isl::basic_set &bSet,
                                             size_t dim);
std::vector<isl::aff> ExtractAffLowerBounds(const isl::basic_set &bSet,
                                            size_t dim);
std::vector<isl::aff> ExtractAffUpperBounds(const isl::basic_set &bSet,
                                            size_t dim);

std::vector<long> ExtractConstantLowerBounds(const isl::basic_set &bSet,
                                             const std::string &dimName);
std::vector<long> ExtractConstantUpperBounds(const isl::basic_set &bSet,
                                             const std::string &dimName);
std::vector<isl::aff> ExtractAffLowerBounds(const isl::basic_set &bSet,
                                            const std::string &dimName);
std::vector<isl::aff> ExtractAffUpperBounds(const isl::basic_set &bSet,
                                            const std::string &dimName);

std::string GetConstraintDimName(isl_constraint *c, size_t dim);

size_t GetSetDimension(const isl::set &set);
size_t GetBasicSetDimension(const isl::basic_set &basic_set);
size_t GetPointDimension(const isl::point &point);
size_t GetMapDomainDimension(const isl::map &map);
size_t GetMapRangeDimension(const isl::map &map);
size_t GetAffDomainDimension(const isl::aff &aff);
size_t GetPwAffDomainDimension(const isl::pw_aff &pwAff);

long GetSetDimStride(const isl::set &set, size_t dim);
long GetSetDimNameStride(const isl::set &set, const std::string &dimName);
long GetPointDimValue(const isl::point &point, size_t dim);

std::string GetSetTupleName(const isl::set &set);
std::string GetMapDomainTupleName(const isl::map &map);
std::string GetMapRangeTupleName(const isl::map &map);
std::string GetSetDimName(const isl::set &set, size_t pos);
std::string GetBasicSetDimName(const isl::basic_set &basicSet, size_t pos);
std::string GetAffDomainDimName(const isl::aff &aff, size_t pos);

size_t GetBasicSetDimNamePos(const isl::basic_set &basicSet,
                             const std::string &dimName);
size_t GetSetDimNamePos(const isl::set &set, const std::string &dimName);

long GetAffDomainDimCoefficient(const isl::aff &aff, size_t pos);
long GetAffConstant(const isl::aff &aff);

void AddSetDimensions(isl::set &set, unsigned int nDims);
void AddMapDomainDimensions(isl::map &map, unsigned int nDims);
void AddMapRangeDimensions(isl::map &map, unsigned int nDims);

std::unordered_map<std::string, std::string>
ExtractSymbolicNameMapping(const isl::map &map);

std::vector<isl::aff> ExtractAffineIndices(const isl::map &taggedReadOrWrite);

isl::set EliminateDimsAfterDim(const isl::set &set, size_t dim);
isl::set DropDimsAfterDim(const isl::set &set, size_t dim);

std::vector<std::string> GetSetDimNames(const isl::set &set);
std::vector<std::string> GetAffDimNames(const isl::aff &aff);
std::vector<std::string> GetPwAffDimNames(const isl::pw_aff &pwAff);

isl::set RenameSetDims(
    const isl::set &set,
    const std::unordered_map<std::string, std::string> &oldToNewNames);
isl::aff RenameAffDims(
    const isl::aff &aff,
    const std::unordered_map<std::string, std::string> &oldToNewNames);

isl::set AppendToSetDimNames(const isl::set &set, const std::string &appendStr);
isl::aff AppendToAffDimNames(const isl::aff &aff, const std::string &appendStr);

std::string GetPwAffRangeString(const isl::pw_aff &pwAff);
std::string GetPwAffConditionsString(const isl::pw_aff &pwAff);
std::string CreateConflictSetStringBasis(const isl::aff &aff1,
                                         const isl::aff &aff2,
                                         const isl::set &dom1,
                                         const isl::set &dom2,
                                         const std::string &appendStr1,
                                         const std::string &appendStr2);

std::string GetConditionString(const isl::set &set);
bool IsVarInConditionString(const std::string &var, const std::string &str);

isl::point CreateSingleDimPoint(isl::ctx ctx, long val);
isl::point CreateSingleDimPoint(isl::ctx ctx, long val,
                                const isl::space &space);
isl::point CreateSingleDimPoint(isl::ctx ctx, long val, const isl::space &space,
                                const std::vector<long> &params);

isl::set CreateEmptySet(const isl::space &space);

isl::basic_set CreateBasicSetFromVal(isl::ctx ctx, long val,
                                     const isl::space &space,
                                     const std::vector<long> &paramVals);

isl::multi_aff GetBasicMapDomainMultiAffIdentity(const isl::basic_map &bMap);

isl::multi_aff GetBasicSetMultiAffIdentity(const isl::basic_set &bSet);

isl::aff GetAffValOnBasicSetSpace(long val, const isl::basic_set &bSet);

isl::basic_set AddBasicSetEqConstraint(const isl::basic_set &bSet,
                                       const isl::multi_aff &mAff, size_t dim,
                                       long val);
isl::basic_set AddBasicSetGreaterEqConstraint(const isl::basic_set &bSet,
                                              const isl::multi_aff &mAff,
                                              size_t dim, long val);
isl::basic_set AddBasicSetLessEqConstraint(const isl::basic_set &bSet,
                                           const isl::multi_aff &mAff,
                                           size_t dim, long val);
isl::basic_set AddBasicSetGreaterConstraint(const isl::basic_set &bSet,
                                            const isl::multi_aff &mAff,
                                            size_t dim, long val);
isl::basic_set AddBasicSetLessConstraint(const isl::basic_set &bSet,
                                         const isl::multi_aff &mAff, size_t dim,
                                         long val);

isl::basic_set AddBasicSetEqConstraint(const isl::basic_set &bSet,
                                       const isl::multi_aff &mAff,
                                       const std::string &dimName, long val);
isl::basic_set AddBasicSetGreaterEqConstraint(const isl::basic_set &bSet,
                                              const isl::multi_aff &mAff,
                                              const std::string &dimName,
                                              long val);
isl::basic_set AddBasicSetLessEqConstraint(const isl::basic_set &bSet,
                                           const isl::multi_aff &mAff,
                                           const std::string &dimName,
                                           long val);
isl::basic_set AddBasicSetGreaterConstraint(const isl::basic_set &bSet,
                                            const isl::multi_aff &mAff,
                                            const std::string &dimName,
                                            long val);
isl::basic_set AddBasicSetLessConstraint(const isl::basic_set &bSet,
                                         const isl::multi_aff &mAff,
                                         const std::string &dimName, long val);

isl::basic_map AddBasicMapDomainEqConstraint(const isl::basic_map &bMap,
                                             const isl::multi_aff &mAff,
                                             size_t dim, long val);
isl::basic_map AddBasicMapDomainGreaterEqConstraint(const isl::basic_map &bMap,
                                                    const isl::multi_aff &mAff,
                                                    size_t dim, long val);
isl::basic_map AddBasicMapDomainLessEqConstraint(const isl::basic_map &bMap,
                                                 const isl::multi_aff &mAff,
                                                 size_t dim, long val);
isl::basic_map AddBasicMapDomainGreaterConstraint(const isl::basic_map &bMap,
                                                  const isl::multi_aff &mAff,
                                                  size_t dim, long val);
isl::basic_map AddBasicMapDomainLessConstraint(const isl::basic_map &bMap,
                                               const isl::multi_aff &mAff,
                                               size_t dim, long val);

isl::basic_map AddBasicMapDomainEqConstraint(const isl::basic_map &bMap,
                                             const isl::multi_aff &mAff,
                                             const std::string &dimName,
                                             long val);
isl::basic_map AddBasicMapDomainGreaterEqConstraint(const isl::basic_map &bMap,
                                                    const isl::multi_aff &mAff,
                                                    const std::string &dimName,
                                                    long val);
isl::basic_map AddBasicMapDomainLessEqConstraint(const isl::basic_map &bMap,
                                                 const isl::multi_aff &mAff,
                                                 const std::string &dimName,
                                                 long val);
isl::basic_map AddBasicMapDomainGreaterConstraint(const isl::basic_map &bMap,
                                                  const isl::multi_aff &mAff,
                                                  const std::string &dimName,
                                                  long val);
isl::basic_map AddBasicMapDomainLessConstraint(const isl::basic_map &bMap,
                                               const isl::multi_aff &mAff,
                                               const std::string &dimName,
                                               long val);

isl::basic_set EvalBasicMap(const isl::basic_map &bMap, long val,
                            const std::vector<long> &paramVals);

isl::basic_set SetBasicSetParamValues(const isl::basic_set &bSet,
                                      const std::vector<long> &paramValues);

isl::basic_set GetBasicMapDomain(const isl::basic_map &bMap);
isl::basic_set GetBasicMapRange(const isl::basic_map &bMap);
isl::space GetBasicMapDomainSpace(const isl::basic_map &bMap);
isl::space GetBasicSetSpace(const isl::basic_set &bSet);

isl::basic_map UnwrapBasicSet(const isl::basic_set &bSet);

isl::basic_map DivAndFloorBasicMap(const isl::basic_map &bMap, long divisor);

isl::basic_map ConvertPwAffToBasicMap(const isl::pw_aff &pwAff);

isl::pw_aff ConvertAffToPwAff(const isl::aff &aff);
isl::pw_aff ConvertAffToPwAff(const isl::aff &aff, const isl::set &domain);

isl::set RemoveSetTupleName(const isl::set &set);
isl::aff RemoveAffTupleName(const isl::aff &aff);
isl::pw_aff RemovePwAffTupleName(const isl::pw_aff &pwAff);

std::string AddIslStringCondition(const std::string &str,
                                  const std::string &condition);

isl::pw_aff RemoveAllConditions(const isl::pw_aff &pwAff);

isl::pw_aff AddPwAffEqualCondition(const isl::pw_aff &pwAff,
                                   const std::string &var, long val);
isl::pw_aff AddPwAffCondition(const isl::pw_aff &pwAff,
                              const std::string &condition);

isl::set AddSetEqualCondition(const isl::set &set, const std::string &var,
                              long val);
isl::set AddSetCondition(const isl::set &set, const std::string &condition);

isl::basic_set AddBasicSetEqualCondition(const isl::basic_set &basicSet,
                                         const std::string &var, long val);
isl::basic_set
AddBasicSetGreaterThanOrEqualToCondition(const isl::basic_set &basicSet,
                                         const std::string &var, long val);
isl::basic_set
AddBasicSetLessThanOrEqualToCondition(const isl::basic_set &basicSet,
                                      const std::string &var, long val);
isl::basic_set AddBasicSetCondition(const isl::basic_set &basicSet,
                                    const std::string &condition);

isl::set ConvertUnionSetToSet(isl::union_set uset);
isl::map ConvertUnionMapToMap(isl::union_map umap);

isl::basic_map ConvertMapToBasicMap(const isl::map &map);

std::vector<long> ConvertPointToVector(const isl::point &point);
std::vector<std::string> ConvertSetTupleToStringVector(const isl::set &set);
std::vector<std::string>
ConvertBasicSetTupleToStringVector(const isl::basic_set &bSet);
std::vector<std::string>
ConvertMapDomainTupleToStringVector(const isl::map &map);

isl::basic_set NegateBasicSet(const isl::basic_set &basicSet);

isl::basic_set ConvertSetToBasicSet(const isl::set &set);

long GetBasicSetDimMaxValue(const isl::basic_set &basicSet, size_t pos);
long GetBasicSetDimMinValue(const isl::basic_set &basicSet, size_t pos);

bool IsBasicSetDimMaxValBounded(const isl::basic_set &bSet, size_t dim);

long ToLong(const isl::val &val);

std::string ConstructIffConditionString(const std::string &cond1,
                                        const std::string &cond2);

std::string NegateConditionString(const std::string &cond);

std::string ReplaceIteratorWithExpressionString(const std::string &itStr,
                                                const std::string &exprStr,
                                                const std::string &condStr);

std::string GetStringWithoutBraces(const isl::set &set);

std::string ToString(const isl::point &point);
std::string ToString(const isl::space &space);
std::string ToString(const isl::set &set);
std::string ToString(const isl::basic_set &bSet);
std::string ToString(const isl::map &map);
std::string ToString(const isl::aff &aff);
std::string ToString(const isl::pw_aff &pwAff);

}; // namespace IslUtil
