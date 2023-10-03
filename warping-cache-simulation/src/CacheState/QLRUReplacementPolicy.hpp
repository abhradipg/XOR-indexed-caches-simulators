#pragma once

#include <functional>

#include "CacheState/ReplacementPolicy.hpp"
#include "Settings/QLRUSettings.hpp"

class QLRUReplacementPolicy : public ReplacementPolicy {
public:
  QLRUReplacementPolicy(size_t nLines,
                        const GlobalIteratorState &globalIteratorState,
                        const QLRUSettings &qlruSettings);

  [[nodiscard]] size_t GetNumberOfNonEmptyLines() const override;

  [[nodiscard]] bool DoesContainMemoryBlock(size_t memoryBlock) const override;

  [[nodiscard]] std::optional<CacheLine>
  AddNewCacheLineAndGetEvicted(const Address &address) override;

  void AccessToExistingCacheLine(const Address &address) override;

  void AccessToExistingCacheLineAndMarkDirty(const Address &address) override;

  void AddLinesToCacheStateSnapshot(
      CacheStateSnapshot &cacheStateSnapshot) const override;

  void Warp(const WarpInfo &warpInfo) override;

private:
  using Age = unsigned short;
  using LineIndexSet = std::set<size_t>;
  using LineInfo = std::pair<CacheLine, LineIndexSet::iterator>;

  const std::function<Age(Age)> hitFunction;

  const std::function<size_t()> replaceFunction;

  const std::function<void(size_t)> updateFunction;

  const Age ageAfterMiss;

  std::optional<size_t> leftMostEmptyLineIndex;
  std::optional<size_t> rightMostEmptyLineIndex;

  mutable std::vector<LineIndexSet> lineIndicesByAge;

  std::vector<std::optional<LineInfo>> lineInfos;
  std::unordered_map<size_t, size_t> memoryBlockToLineIndex;

  void AddLinesToCacheStateSnapshotInIndexOrder(
      CacheStateSnapshot &cacheStateSnapshot) const;

  void AddLinesToCacheStateSnapshotInAgeOrder(
      CacheStateSnapshot &cacheStateSnapshot) const;

  [[nodiscard]] bool IsLineEmpty(size_t lineIndex) const;

  [[nodiscard]] Age GetCurrentAge(size_t lineIndex) const;

  [[nodiscard]] LineInfo &GetLineInfoRef(size_t lineIndex);

  [[nodiscard]] std::optional<LineInfo> &GetOptLineInfoRef(size_t lineIndex);

  [[nodiscard]] size_t GetMemoryBlockAtLineIndex(size_t lineIndex);

  void AccessNonEmptyLineIndexOnMiss(size_t lineIndex, const Address &address);

  void AccessNonEmptyLineIndexOnHit(size_t lineIndex, const Address &address);

  void AccessNonEmptyLineIndexAndMarkDirtyOnHit(size_t lineIndex,
                                                const Address &address);

  void AccessNonEmptyLineIndexAndChangeItsAge(size_t lineIndex,
                                              const Address &address,
                                              Age currentAge, Age newAge);

  void AccessNonEmptyLineIndexAndMarkDirtyAndChangeItsAge(
      size_t lineIndex, const Address &address, Age currentAge, Age newAge);

  void ChangeMemoryBlockToLineIndexKey(size_t oldKey, size_t newKey);

  void InsertToMemoryBlockToLineIndex(size_t memoryBlock, size_t lineIndex);

  void AccessLeftMostEmptyLineIndex(const Address &address);

  void AccessRightMostEmptyLineIndex(const Address &address);

  void RefreshLeftMostEmptyLineIndex();

  void RefreshRightMostEmptyLineIndex();

  void FillEmptyLineIndex(size_t lineIndex, const Address &address);

  void EraseAgeForLineIndex(Age age, const LineIndexSet::iterator &it);

  void ShiftAllAges(Age ageShift);

  void ShiftAgesExceptLine(Age ageShift, size_t lineIndex);

  [[nodiscard]] bool IsLineIndexEmpty(size_t lineIndex) const;

  [[nodiscard]] LineIndexSet::iterator InsertAgeForLineIndex(size_t lineIndex,
                                                             Age age);

  [[nodiscard]] Age GetMaxAge() const;

  [[nodiscard]] size_t GetNonEmptyLineIndex(const Address &address) const;

  [[nodiscard]] std::function<Age(Age)>
  CreateHitFunction(const QLRUSettings &qlruSettings) const;

  [[nodiscard]] std::function<size_t()>
  CreateReplaceFunction(const QLRUSettings &qlruSettings);

  [[nodiscard]] std::function<void(size_t)>
  CreateUpdateFunction(const QLRUSettings &qlruSettings);

  [[nodiscard]] std::function<size_t()> CreateReplaceFunctionR0();

  [[nodiscard]] std::function<size_t()> CreateReplaceFunctionR1();

  [[nodiscard]] std::function<size_t()> CreateReplaceFunctionR2();

  [[nodiscard]] std::function<void(size_t)> CreateUpdateFunctionU0();

  [[nodiscard]] std::function<void(size_t)> CreateUpdateFunctionU1();

  [[nodiscard]] std::function<void(size_t)> CreateUpdateFunctionU2();

  [[nodiscard]] std::function<void(size_t)> CreateUpdateFunctionU3();
};
