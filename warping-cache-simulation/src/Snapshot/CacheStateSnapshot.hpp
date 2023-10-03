#pragma once

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include "Address/Address.hpp"
#include "CacheState/CacheLine.hpp"
#include "IteratorState/IteratorStateMap.hpp"
#include "Util/Util.hpp"

class CacheStateSnapshot {
public:
  CacheStateSnapshot(size_t nLines, size_t nCacheSets, size_t itId);

  void AddLine(const CacheLine &line,
               const GlobalIteratorState &globalIteratorState);

  void SetLastAccessedSetIndex(size_t index);

  [[nodiscard]] bool IsSymbolicallyEqual(const CacheStateSnapshot &other) const;

  [[nodiscard]] size_t GetConcreteAddress(size_t lineIndex) const;

  [[nodiscard]] const std::vector<CacheLine> &GetLines() const;

  [[nodiscard]] bool DoesHaveSingleCoefficient() const;

  [[nodiscard]] long GetSingleCoefficient() const;

  [[nodiscard]] size_t GetSymbolicHash() const;

  [[nodiscard]] size_t GetLastAccessedSetIndex() const;

  void WarpCacheLine(size_t lineIndex, long concreteAddressDiff);

  void Rotate(size_t nRightRotations);

private:
  const size_t changingItId;

  const size_t nCacheSets;

  size_t symbolicHash;

  std::vector<CacheLine> lines;

  size_t lastAccessedSetIndex = 0;

  bool hasSingleCoefficient = false;

  std::optional<long> singleCoefficient = std::nullopt;

  void
  ComputeAndCombineLineHash(const CacheLine &line,
                            const GlobalIteratorState &globalIteratorState);

  void InitCoefficientInfo(const CacheLine &line);

  void UpdateCoefficientInfo(const CacheLine &line);
};
