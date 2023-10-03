#pragma once

#include <cstddef>
#include <iostream>
#include <optional>

#include "Settings/QLRUSettings.hpp"
#include "Util/StrUtil.hpp"

#define DEFAULT_CACHE_SIZE 1024
#define DEFAULT_LINE_SIZE 8
#define DEFAULT_ASSOCIATIVITY 128
#define DEFAULT_N_CACHE_LINES (DEFAULT_CACHE_SIZE / DEFAULT_LINE_SIZE)
#define DEFAULT_N_CACHE_SETS (DEFAULT_N_CACHE_LINES / DEFAULT_ASSOCIATIVITY)
#define DEFAULT_CACHE_LEVEL 0
#define DEFAULT_REPLACEMENT_POLICY ReplacementPolicy::LRU
#define DEFAULT_WRITE_POLICY WritePolicy::WRITE_BACK_WRITE_ALLOCATE

struct CacheSettings {
public:
  size_t cacheSize = DEFAULT_CACHE_SIZE;

  size_t lineSize = DEFAULT_LINE_SIZE;

  size_t associativity = DEFAULT_ASSOCIATIVITY;

  size_t nCacheLines = DEFAULT_N_CACHE_LINES;

  size_t nCacheSets = DEFAULT_N_CACHE_SETS;

  size_t cacheLevel = DEFAULT_CACHE_LEVEL;

  std::optional<QLRUSettings> optQLRUSettings;

  enum ReplacementPolicy {
    LRU,
    PLRU,
    QLRU,
    LRU3PLRU4,
    FIFO
  } replacementPolicy = DEFAULT_REPLACEMENT_POLICY;

  enum WritePolicy {
    WRITE_BACK_WRITE_ALLOCATE,
    WRITE_THROUGH_NO_WRITE_ALLOCATE
  } writePolicy = DEFAULT_WRITE_POLICY;

  [[nodiscard]] bool IsValid() const;

  [[nodiscard]] bool IsLRU() const;

  [[nodiscard]] bool IsPLRU() const;

  [[nodiscard]] bool IsQLRU() const;

  [[nodiscard]] bool IsLRU3PLRU4() const;

  [[nodiscard]] bool IsFIFO() const;

  [[nodiscard]] bool IsWriteBackWriteAllocate() const;

  [[nodiscard]] bool IsWriteThroughNoWriteAllocate() const;

  [[nodiscard]] bool IsFullyAssociative() const;

  [[nodiscard]] bool IsSetAssociative() const;

  [[nodiscard]] std::string GetReplacementPolicyJSONStr() const;

  [[nodiscard]] std::string GetWritePolicyJSONStr() const;
};
