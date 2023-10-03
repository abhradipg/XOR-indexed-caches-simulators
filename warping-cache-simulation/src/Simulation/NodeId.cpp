#include "NodeId.hpp"

NodeId::NodeId(std::string id) : str(std::move(id)) {
  this->vec = StrUtil::SplitStringByChar(this->str, NodeId::delimiter);
  this->hash = std::hash<std::string>()(this->str);
}

NodeId::NodeId(size_t dim, const StatementInfo &si)
    : NodeId(this->ConstructIdStr(dim, si)) {}

NodeId NodeId::CreateAccessNodeId() const {
  return NodeId(this->str + NodeId::delimiter + "0");
}
const std::string &NodeId::GetAsString() const { return this->str; }

const std::vector<std::string> &NodeId::GetAsVector() const {
  return this->vec;
}

size_t NodeId::GetHash() const { return this->hash; }

bool NodeId::HasParent() const {
  auto it =
      std::find_if_not(this->vec.begin(), this->vec.end(), StrUtil::IsInteger);
  return !(it == this->vec.end() || std::next(it) == this->vec.end());
}

bool NodeId::IsIterationNodeId() const {
  return !StrUtil::IsInteger(this->vec.back());
}

const std::string &NodeId::GetIteratorName() const {
  assert(this->IsIterationNodeId());
  return this->vec.back();
}

size_t NodeId::GetIteratorDim() const {
  assert(this->IsIterationNodeId());
  return this->GetAsVector().size() - 1;
}

NodeId NodeId::GetClosestParentId() const {
  assert(this->HasParent());
  size_t lastIndex = this->vec.size();
  for (size_t i = 0; i < this->vec.size() - 1; i++)
    if (!StrUtil::IsInteger(this->vec[i]))
      lastIndex = i;

  assert(lastIndex != this->vec.size());
  assert(this->vec[lastIndex] != this->vec.back());

  std::string parentIdStr = this->vec[0];
  for (size_t i = 1; i <= lastIndex; i++)
    parentIdStr.append(NodeId::delimiter + this->vec[i]);

  return NodeId(parentIdStr);
}

std::string NodeId::ConstructIdStr(size_t dim, const StatementInfo &si) const {
  const auto &names = si.GetTimestampStrDims();
  assert(!names.empty());
  std::string idStr = names[0];
  for (size_t pos = 1; pos <= dim; pos++)
    idStr.append(NodeId::delimiter + names[pos]);
  return idStr;
}

bool NodeId::operator<(const NodeId &other) const {
  // if (this->vec.size() != other.vec.size())
  //  return this->vec.size() < other.vec.size();

  size_t nIndices = std::min(this->vec.size(), other.vec.size());
  for (size_t i = 0; i < nIndices; i++) {
    const auto &el1 = this->vec[i];
    const auto &el2 = other.vec[i];
    if (StrUtil::IsInteger(el1)) {
      assert(StrUtil::IsInteger(el2));
      const auto val1 = std::stol(el1);
      const auto val2 = std::stol(el2);
      if (val1 == val2)
        continue;
      return val1 < val2;
    } else
      assert(el1 == el2);
  }

  return this->vec.size() < other.vec.size();
}

bool NodeId::operator==(const NodeId &other) const {
  return this->hash == other.hash;
}

bool NodeId::operator!=(const NodeId &other) const { return !(*this == other); }
