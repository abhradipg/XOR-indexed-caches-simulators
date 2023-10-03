#pragma once

#include <cassert>
#include <cstddef>
#include <list>
#include <unordered_map>

/*
 *  First element -> most-recently-used (MRU)
 *  Last element -> least-recently-used (LRU)
 */
template <class T> class LRUContainer {
public:
  [[nodiscard]] bool DoesContain(size_t id) const;

  [[nodiscard]] bool IsEmpty() const;

  [[nodiscard]] size_t GetSize() const;

  [[nodiscard]] const std::list<std::pair<size_t, T>> &
  GetIdElPairsConstRef() const;

  std::list<std::pair<size_t, T>> &GetIdElPairsRef();

  std::unordered_map<size_t, typename std::list<std::pair<size_t, T>>::iterator>
      &GetIdElPairItMapRef();

  [[nodiscard]] const T &GetLruElConstRef() const;

  [[nodiscard]] const T &GetMruElConstRef() const;

  [[nodiscard]] const std::pair<size_t, T> &GetLruIdElPairConstRef() const;

  [[nodiscard]] const std::pair<size_t, T> &GetMruIdElPairConstRef() const;

  void Use(size_t id);

  void UseLru();

  [[nodiscard]] T &UseLruAndGetUsedElRef();

  [[nodiscard]] T &UseAndGetUsedElRef(size_t id);

  [[nodiscard]] std::pair<size_t, T> &UseAndGetUsedIdElPairRef(size_t id);

  void Add(size_t id, const T &el);

  void Remove(size_t id);

  void RemoveLru();

private:
  std::list<std::pair<size_t, T>> idElPairs;
  std::unordered_map<size_t, typename decltype(idElPairs)::iterator>
      idElPairItMap;
};

template <class T> bool LRUContainer<T>::DoesContain(size_t id) const {
  return this->idElPairItMap.count(id) == 1;
}

template <class T> bool LRUContainer<T>::IsEmpty() const {
  return this->GetSize() == 0;
}

template <class T> size_t LRUContainer<T>::GetSize() const {
  assert(this->idElPairs.size() == this->idElPairItMap.size());
  return this->idElPairs.size();
}

template <class T>
const std::list<std::pair<size_t, T>> &
LRUContainer<T>::GetIdElPairsConstRef() const {
  return this->idElPairs;
}

template <class T>
std::list<std::pair<size_t, T>> &LRUContainer<T>::GetIdElPairsRef() {
  return this->idElPairs;
}

template <class T>
std::unordered_map<size_t, typename std::list<std::pair<size_t, T>>::iterator> &
LRUContainer<T>::GetIdElPairItMapRef() {
  return this->idElPairItMap;
}

template <class T> const T &LRUContainer<T>::GetLruElConstRef() const {
  return this->idElPairs.back().second;
}

template <class T> const T &LRUContainer<T>::GetMruElConstRef() const {
  return this->idElPairs.front().second;
}

template <class T>
const std::pair<size_t, T> &LRUContainer<T>::GetLruIdElPairConstRef() const {
  return this->idElPairs.back();
}

template <class T>
const std::pair<size_t, T> &LRUContainer<T>::GetMruIdElPairConstRef() const {
  return this->idElPairs.front();
}

template <class T> void LRUContainer<T>::Use(size_t id) {
  auto it = this->idElPairItMap.find(id);
  this->idElPairs.splice(this->idElPairs.begin(), this->idElPairs, it->second);
}

template <class T> void LRUContainer<T>::UseLru() {
  assert(!this->idElPairs.empty());
  auto it = std::prev(this->idElPairs.end());
  this->idElPairs.splice(this->idElPairs.begin(), this->idElPairs, it);
}

template <class T> T &LRUContainer<T>::UseLruAndGetUsedElRef() {
  this->UseLru();
  return this->idElPairs.front().second;
}

template <class T> T &LRUContainer<T>::UseAndGetUsedElRef(size_t id) {
  return this->UseAndGetUsedIdElPairRef(id).second;
}

template <class T>
std::pair<size_t, T> &LRUContainer<T>::UseAndGetUsedIdElPairRef(size_t id) {
  this->Use(id);
  return this->idElPairs.front();
}

template <class T> void LRUContainer<T>::Add(size_t id, const T &el) {
  assert(this->idElPairItMap.count(id) == 0);
  this->idElPairs.emplace_front(id, el);
  this->idElPairItMap.emplace(id, this->idElPairs.begin());
}

template <class T> void LRUContainer<T>::Remove(size_t id) {
  auto it = this->idElPairItMap.find(id);
  assert(it != this->idElPairItMap.end());
  this->idElPairs.erase(it->second);
  this->idElPairItMap.erase(it);
}

template <class T> void LRUContainer<T>::RemoveLru() {
  assert(!this->IsEmpty());
  const auto lruId = this->GetLruIdElPairConstRef().first;
  auto it = this->idElPairItMap.find(lruId);
  assert(it != this->idElPairItMap.end());
  this->idElPairs.pop_back();
  this->idElPairItMap.erase(it);
}
