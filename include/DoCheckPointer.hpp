#pragma once

#include <stdexcept>

template<typename PointerType>
bool doCheckPointer(PointerType* pointer, PointerType* closedValue, bool throwException, const char* message = "")
{
  if(!throwException) return (pointer == closedValue);
  if(pointer == closedValue)
  {
    throw std::runtime_error(message);
    return true; // to disable IDE's warning
  }
  else return false;
}

class ClosableInstance
{
public:
  virtual void close() = 0;
  virtual bool closed() const { return doCheckClosed(false); };
protected:
  virtual bool doCheckClosed(bool throwException) const = 0;
};
