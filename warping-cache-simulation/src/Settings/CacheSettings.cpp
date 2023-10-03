#include "CacheSettings.hpp"

bool CacheSettings::IsValid() const {
  if (this->cacheSize % this->lineSize != 0) {
    std::cerr << "-> Cache size is not a multiple of line size!" << std::endl;
    return false;
  }
  if ((this->cacheSize / this->lineSize) % this->associativity != 0) {
    std::cerr << "-> Cache size / line size is not a multiple of associativity!"
              << std::endl;
    return false;
  }
  if ((this->cacheSize / this->lineSize) != this->nCacheLines) {
    std::cerr << "-> Number of cache lines is set wrong!" << std::endl;
    return false;
  }
  if ((this->cacheSize / this->lineSize) / this->associativity !=
      this->nCacheSets) {
    std::cerr << "-> Number of cache sets is set wrong!" << std::endl;
    return false;
  }
  return true;
}

bool CacheSettings::IsLRU() const { return this->replacementPolicy == LRU; }

bool CacheSettings::IsPLRU() const { return this->replacementPolicy == PLRU; }

bool CacheSettings::IsQLRU() const { return this->replacementPolicy == QLRU; }

bool CacheSettings::IsLRU3PLRU4() const {
  return this->replacementPolicy == LRU3PLRU4;
}

bool CacheSettings::IsFIFO() const { return this->replacementPolicy == FIFO; }

bool CacheSettings::IsWriteBackWriteAllocate() const {
  return this->writePolicy == WRITE_BACK_WRITE_ALLOCATE;
}

bool CacheSettings::IsWriteThroughNoWriteAllocate() const {
  return this->writePolicy == WRITE_THROUGH_NO_WRITE_ALLOCATE;
}

bool CacheSettings::IsFullyAssociative() const { return this->nCacheSets == 1; }

bool CacheSettings::IsSetAssociative() const { return this->nCacheSets > 1; }

std::string CacheSettings::GetReplacementPolicyJSONStr() const {
  if (this->IsLRU())
    return "\"lru\"";
  else if (this->IsPLRU())
    return "\"plru\"";
  else if (this->IsQLRU()) {
    assert(this->optQLRUSettings);
    return StrUtil::Format("\"%s\"", this->optQLRUSettings->ToString().c_str());
  } else if (this->IsLRU3PLRU4())
    return "\"lru3plru4\"";
  else if (this->IsFIFO())
    return "\"fifo\"";
  return "\"error\"";
}

std::string CacheSettings::GetWritePolicyJSONStr() const {
  if (this->IsWriteBackWriteAllocate())
    return "\"write-back-write-allocate\"";
  else if (this->IsWriteThroughNoWriteAllocate())
    return "\"write-through-no-write-allocate\"";
  return "\"error\"";
}