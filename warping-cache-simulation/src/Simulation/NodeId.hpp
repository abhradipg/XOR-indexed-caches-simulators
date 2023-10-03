#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "ProgramInfo/StatementInfo.hpp"
#include "Util/StrUtil.hpp"
#include "Util/Util.hpp"

class NodeId {
public:
  explicit NodeId(std::string id);

  NodeId(size_t dim, const StatementInfo &si);

  [[nodiscard]] NodeId CreateAccessNodeId() const;

  [[nodiscard]] const std::string &GetAsString() const;

  [[nodiscard]] const std::vector<std::string> &GetAsVector() const;

  [[nodiscard]] size_t GetHash() const;

  [[nodiscard]] bool HasParent() const;

  [[nodiscard]] bool IsIterationNodeId() const;

  [[nodiscard]] const std::string &GetIteratorName() const;

  [[nodiscard]] size_t GetIteratorDim() const;

  [[nodiscard]] NodeId GetClosestParentId() const;

  [[nodiscard]] bool operator<(const NodeId &other) const;
  [[nodiscard]] bool operator==(const NodeId &other) const;
  [[nodiscard]] bool operator!=(const NodeId &other) const;

private:
  static constexpr char delimiter = '_';

  std::string str;
  std::vector<std::string> vec;
  size_t hash;

  [[nodiscard]] std::string ConstructIdStr(size_t dim,
                                           const StatementInfo &si) const;
};

namespace std {
template <> struct hash<NodeId> {
  size_t operator()(const NodeId &nid) const { return nid.GetHash(); }
};
} // namespace std
