#include "AccessInfo.hpp"

AccessInfo::AccessInfo(bool isRead, const ArrayInfo &arrayInfo,
                       const isl::map &taggedReadOrWrite)
    : isRead(isRead), arrayInfo((arrayInfo)) {
  this->refId =
      IslUtil::GetMapRangeTupleName(taggedReadOrWrite.domain().unwrap());

  const auto &affineIndices = IslUtil::ExtractAffineIndices(taggedReadOrWrite);
  this->affineAddress =
      (IslUtil::RemoveAffTupleName(arrayInfo.CreateAffAddress(affineIndices)));
  this->domain =
      IslUtil::RemoveSetTupleName(taggedReadOrWrite.domain().unwrap().domain());
}

void AccessInfo::RenameSymbolicVars(
    const std::unordered_map<std::string, std::string> &newNames) {
  this->affineAddress = IslUtil::RenameAffDims(this->affineAddress, newNames);
  this->domain = IslUtil::RenameSetDims(this->domain, newNames);
}

isl::ctx AccessInfo::GetIslCtx() const { return this->domain.ctx(); }

const isl::set &AccessInfo::GetDomain() const { return this->domain; }

const isl::aff &AccessInfo::GetAffineAddress() const {
  return this->affineAddress;
}

std::pair<size_t, size_t>
AccessInfo::GetAccessedArrayMemoryBlockInterval() const {
  const auto startingAddress = this->arrayInfo.GetStartingAddress();
  const auto start = Util::ComputeMemoryBlock(startingAddress);
  const auto end =
      Util::ComputeMemoryBlock(startingAddress + this->arrayInfo.GetSize());
  assert(start <= end);
  return {start, end};
}

bool AccessInfo::IsRead() const { return this->isRead; }

bool AccessInfo::operator<(const AccessInfo &other) const {
  if (this->isRead != other.isRead)
    return this->isRead;

  if (this->refId.length() == other.refId.length())
    return this->refId < other.refId;

  return this->refId.length() < other.refId.length();
}
