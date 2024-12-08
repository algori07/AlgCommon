#pragma once

#include <map>
#include <unordered_map>
#include <initializer_list>
#include <utility>
#include <memory>
#include <stdexcept>


template<typename TypeFirst, typename TypeSecond,
         typename MapFirstType = std::map<TypeFirst, const TypeSecond*>,
         typename MapSecondType = std::map<TypeSecond, const TypeFirst*>>
class BiMap
{
public:
  class ConstIterator;
  using Iterator = ConstIterator;
  using ValueType = std::pair<TypeFirst, TypeSecond>;
  using ValueReferenceType = std::pair<const TypeFirst&, const TypeSecond&>;
public:
  BiMap();
  BiMap(const std::initializer_list<ValueType>& list);
  
  std::pair<ConstIterator, bool> insert(const ValueType& value )
    { return this->insert(value.first, value.second); }
  std::pair<ConstIterator, bool> insert(const TypeFirst& valueA, const TypeSecond& valueB);
  
  const TypeSecond& atFirst(const TypeFirst& key) const;
  const TypeFirst& atSecond(const TypeSecond& key) const;
  
  ConstIterator findFirst(const TypeFirst& key) const;
  ConstIterator findSecond(const TypeSecond& key) const;
  
  ConstIterator erase(ConstIterator pos);
  ConstIterator erase(ConstIterator first, ConstIterator last);
  size_t eraseFirst(const TypeFirst& key);
  size_t eraseSecond(const TypeSecond& key);
  
  void clear();
  
  ConstIterator begin() const { return ConstIterator(m_mapFirst.begin()); }
  ConstIterator end() const { return ConstIterator(m_mapFirst.end()); }
  
private:
  
  using MapFirstIterator = typename MapFirstType::iterator;
  using MapFirstConstIterator = typename MapFirstType::const_iterator;
  MapFirstType m_mapFirst;
  
  using MapSecondIterator = typename MapSecondType::iterator;
  using MapSecondConstIterator = typename MapSecondType::const_iterator;
  MapSecondType m_mapSecond;
};

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
class BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>::ConstIterator
{
private:
  class Proxy
  {
  public:
    Proxy(const ValueType& value): m_value(value) {}
    ValueReferenceType* operator->() { return &m_value; }
  private:
    ValueReferenceType m_value;
  };
public:
  ConstIterator() {}
  ConstIterator(MapFirstConstIterator iter)
    : m_iter(iter) {}
//  ConstIterator(MapSecondConstIterator iter)
//    : m_iter(m_mapFirst.find(*(iter->second))) {}
  
  ValueReferenceType operator*() const { return {m_iter->first, *(m_iter->second)}; }
  Proxy operator->() const { return Proxy(*(*this)); } // call operator * of this pointer
  
  ConstIterator operator++() { ++m_iter; return *this; }
  ConstIterator operator--() { --m_iter; return *this; }
  bool operator!=(const ConstIterator& other) const { return m_iter != other.m_iter; }
private:
  MapFirstConstIterator m_iter;
};


template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::BiMap() { }

template<typename TypeFirst, typename TypeSecond, typename MapFirstType, typename MapSecondType>
  BiMap<TypeFirst, TypeSecond, MapFirstType, MapSecondType>
  ::BiMap(const std::initializer_list<ValueType>& list)
{
  for(const ValueType& value : list)
  {
    this->insert(value);
  }
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
std::pair<typename BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>::ConstIterator, bool>
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::insert(const TypeFirst& valueFirst, const TypeSecond& valueSecond)
{
  if(m_mapFirst.count(valueFirst) != 0) return {this->end(),false};
  if(m_mapSecond.count(valueSecond) != 0) return {this->end(),false};
  
  MapFirstIterator itFirst = m_mapFirst.insert({valueFirst, nullptr}).first;
  MapSecondIterator itSecond = m_mapSecond.insert({valueSecond, nullptr}).first;
  
  itFirst->second = &(itSecond->first);
  itSecond->second = &(itFirst->first);
  
  return {ConstIterator(itFirst),true};
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
const TypeSecond&
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::atFirst(const TypeFirst& key) const
{
  try {
    const TypeSecond* it = m_mapFirst.at(key);
    return *it;
  }  catch (std::out_of_range exception) {
    throw std::out_of_range("invalid BiMap<TypeFirst,TypeSecond> TypeFirst key");
  }
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
const TypeFirst&
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::atSecond(const TypeSecond& key) const
{
  try {
    const TypeFirst* it = m_mapSecond.at(key);
    return *it;
  }  catch (std::out_of_range exception) {
    throw std::out_of_range("invalid BiMap<TypeFirst,TypeSecond> TypeSecond key");
  }
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
typename BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
::ConstIterator
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::findFirst(const TypeFirst& key) const
{
  return ConstIterator(m_mapFirst.find(key));
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
typename BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
::ConstIterator
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::findSecond(const TypeSecond& key) const
{
  MapSecondConstIterator itSecond = m_mapSecond.find(key);
  if(itSecond == m_mapSecond.end()) return this->end();
  
  MapFirstConstIterator itFirst = m_mapFirst.find(*(itSecond->second));
  return ConstIterator(itFirst);
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
typename BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
::ConstIterator
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::erase(ConstIterator pos)
{
  m_mapSecond.erase(*(pos->second));
  return ConstIterator(m_mapFirst.erase(pos.m_iter));
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
typename BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
::ConstIterator
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::erase(ConstIterator first, ConstIterator last)
{
  for(ConstIterator it = first; it != last; it++)
  {
    m_mapSecond.erase(*(it->second));
  }
  return ConstIterator(m_mapFirst.erase(first.m_iter, last.m_iter));
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
size_t
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::eraseFirst(const TypeFirst& key)
{
  MapFirstConstIterator it = m_mapFirst.find(key);
  if(it == m_mapFirst.end()) return 0;
  m_mapSecond.erase(*(it->second));
  m_mapFirst.erase(it);
  return 1;
}

template<typename TypeFirst, typename TypeSecond, typename MapTypeFirst, typename MapTypeSecond>
size_t
  BiMap<TypeFirst, TypeSecond, MapTypeFirst, MapTypeSecond>
  ::eraseSecond(const TypeSecond& key)
{
  MapSecondConstIterator it = m_mapSecond.find(key);
  if(it == m_mapSecond.end()) return 0;
  m_mapFirst.erase(*(it->second));
  m_mapSecond.erase(it);
  return 1;
}

template<typename TypeFirst, typename TypeSecond, typename MapFirstType, typename MapSecondType>
void
  BiMap<TypeFirst, TypeSecond, MapFirstType, MapSecondType>
  ::clear()
{
  m_mapFirst.clear();
  m_mapSecond.clear();
}

template<typename TypeFirst, typename TypeSecond>
using UnorderedBiMap =
  BiMap<TypeFirst, TypeSecond,
  std::unordered_map<TypeFirst, const TypeSecond*>,
  std::unordered_map<TypeSecond, const TypeFirst*>>;
