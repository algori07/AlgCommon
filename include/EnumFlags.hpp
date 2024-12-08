#pragma once

#include <type_traits>
#include <map>

template<typename EnumType>
class EnumFlags
{
public:
  static_assert (std::is_enum<EnumType>::value, "Only enum type is supported.");
  
  using IntType = typename std::underlying_type<EnumType>::type;
  
  class Iterator
  {
  public:
    Iterator(): m_value(0) {}
    Iterator(EnumType value): m_value((IntType)value) {}
    EnumType operator*() { return (EnumType)(m_value & (~(m_value - 1))); }
    Iterator& operator++() { m_value &= (m_value - 1); return *this; }
    bool operator!=(const Iterator& other) { return m_value != other.m_value; }
  private:
    IntType m_value;
  };
  using iterator = Iterator;
  
  EnumFlags(): m_value((IntType)0) {}
  EnumFlags(const EnumType& flag): m_value((IntType)flag) {}
  EnumFlags(const IntType& flag): m_value(flag) {}
  
  inline EnumFlags operator~() const { return (EnumFlags)(~ m_value); }
  inline EnumFlags operator|(const EnumFlags<EnumType>& flag) const { return (EnumFlags<EnumType>)(m_value | flag.toIntType()); }
  inline EnumFlags operator&(const EnumFlags<EnumType>& flag) const { return (EnumFlags<EnumType>)(m_value & flag.toIntType()); }
  inline EnumFlags operator^(const EnumFlags<EnumType>& flag) const { return (EnumFlags<EnumType>)(m_value ^ flag.toIntType()); }
  inline EnumFlags operator<<(int num) const { return (EnumFlags<EnumType>)(m_value << num); }
  inline EnumFlags operator>>(int num) const { return (EnumFlags<EnumType>)(m_value >> num); }
  
  inline EnumFlags& operator|=(const EnumFlags<EnumType>& flag) { m_value |= flag.toIntType(); return *this; }
  inline EnumFlags& operator&=(const EnumFlags<EnumType>& flag) { m_value &= flag.toIntType(); return *this; }
  inline EnumFlags& operator^=(const EnumFlags<EnumType>& flag) { m_value ^= flag.toIntType(); return *this; }
  inline EnumFlags& operator<<=(int num) { m_value <<= num; return *this; }
  inline EnumFlags& operator>>=(int num) { m_value >>= num; return *this; }
  
  inline bool operator==(IntType val) const { return m_value == val; }
  inline bool operator!=(IntType val) const { return m_value != val; }
  
  inline operator EnumType() const { return (EnumType)(m_value); }
  inline EnumType toEnumType() const { return (EnumType)(m_value); }
  
  inline operator IntType() const { return m_value; }
  inline IntType toIntType() const { return m_value; }
  
  inline bool isEmpty() const { return !(bool)(m_value); }
  
  template<typename FlagOut, FlagOut zeroValue = 0>
  FlagOut mapToRaw(const std::map<EnumType, FlagOut>& map) const
  {
    IntType flags = m_value;
    FlagOut flagsOut = zeroValue;
    for(int i = 0; flags != 0; i++)
    {
      EnumType flag = (EnumType)(flags & (~(flags - 1)));
      flags &= (flags - 1);
      flagsOut |= map.at(flag);
    }
    return flagsOut;
  }
  
  // FlagOut type must be enum type
  template<typename FlagOut, FlagOut zeroValue = 0>
  EnumFlags<FlagOut> mapToEnumFlags(const std::map<EnumType, FlagOut>& map) const
  {
    return EnumFlags<FlagOut>(this->mapToRaw());
  }
  
  Iterator begin() const { return Iterator(m_value); }
  Iterator end() const { return Iterator(0); }
  
  
  
private:
  IntType m_value;
};

// use anywhere you want to declare flags
#define DECLARE_FLAGS(flags, flag) \
  using flags = EnumFlags<flag>;

// use outside class or struct
#define DECLARE_OPERATOR_FOR_FLAGS(flag) \
  inline EnumFlags<flag> operator~(const flag& fa) { return (~ EnumFlags<flag>(fa)); } \
  inline EnumFlags<flag> operator|(const flag& fa, const flag& fb) { return EnumFlags<flag>(fa) | EnumFlags<flag>(fb); } \
  inline EnumFlags<flag> operator&(const flag& fa, const flag& fb) { return EnumFlags<flag>(fa) & EnumFlags<flag>(fb); } \
  inline EnumFlags<flag> operator^(const flag& fa, const flag& fb) { return EnumFlags<flag>(fa) ^ EnumFlags<flag>(fb); } \
  inline EnumFlags<flag> operator<<(flag f, const int& num) { return EnumFlags<flag>(f) << num; } \
  inline EnumFlags<flag> operator>>(flag f, const int& num) { return EnumFlags<flag>(f) >> num; } \

