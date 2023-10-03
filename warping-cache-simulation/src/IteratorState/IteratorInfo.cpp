#include "IteratorInfo.hpp"

IteratorInfo::IteratorInfo(size_t id, std::string name, std::string uniqueName,
                           long stride)
    : id(id), name(std::move(name)), uniqueName(std::move(uniqueName)),
      stride(stride) {
  assert(this->uniqueName == this->name + "_" + std::to_string(this->id));
}

size_t IteratorInfo::GetId() const { return this->id; }

const std::string &IteratorInfo::GetName() const { return this->name; }

const std::string &IteratorInfo::GetUniqueName() const {
  return this->uniqueName;
}

long IteratorInfo::GetStride() const { return this->stride; }
