#ifndef _CACHE_MODEL_H_
#define _CACHE_MODEL_H_

#include <map>
#include <string>
#include <vector>

#include "Access.h"
#include "Definitions.h"
#include "Program.h"
#include <isl/cpp2.h>

// allocate context with include paths
isl_ctx *allocateContextWithIncludePaths(std::vector<std::string> IncludePaths);

// main class of the cache model
class CacheModel {
public:
  CacheModel() = delete;
  CacheModel(const CacheModel &other) = default;
  CacheModel(isl::ctx Context, machine_model MachineModel)
      : Context_(Context), MachineModel_(MachineModel), Program_(Context, MachineModel){};
  CacheModel(isl::ctx Context, machine_model MachineModel, Program P)
      : Context_(Context), MachineModel_(MachineModel), Program_(P){};

  void compileProgram(std::string SourceFile);

  // initialize the cache model
  void initModel(std::vector<NamedLong> Parameters);
  void initModel();

  // prepare the between maps
  void computeBetweenAndFirstMaps();

  // count the actual performance info
  std::vector<NamedMisses> countCacheMisses();
  std::vector<NamedVector> countCacheMisses(std::vector<long> CacheSizes);

  std::vector<Access> getAccesses() const { return Accesses_; }
  std::map<std::string, std::vector<std::string>> getConflicts() const { return Conflicts_; }

private:
  void computeGlobalMaps();
  void extractAccesses();
  void addConflicts(isl::union_map BetweenMap);

  // parameters
  isl::ctx Context_;
  machine_model MachineModel_;

  // program information
  Program Program_;

  // analysis results
  isl::set Parameters_;
  std::vector<NamedLong> ParameterValues_;
  isl::union_map Schedule_;
  isl::map LexSuccEq_;
  isl::union_map SameLineSucc_;
  isl::union_map Before_;
  isl::union_map Forward_;
  isl::union_map First_;

  std::vector<Access> Accesses_;
  std::map<std::string, std::vector<std::string>> Conflicts_;
};

#endif