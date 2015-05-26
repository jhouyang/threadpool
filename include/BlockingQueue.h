#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include "boost/noncopyable.hpp"

// Use this trait to make sure we parse const T& or const T*
template <typename T>
struct ConstRefTrait {
  typedef const T& ConRefType;
  typedef T& RefType;
};

template <typename T>
struct ConstRefTrait<T*> {
  typedef const T* ConRefType;
  typedef T*& RefType;
};

// XXX : long is enough ?
typedef unsigned long TimeUnit;

template <typename T>
class BlockingQueue : private boost::nocopyable
{
  typedef T ElementType;
  typedef typename ConstRefTrait<T>::ConRefType ParamType;
  typedef typename ConstRefTrait<T>::RefType RefType;
public:
  virtual ~BlockingQueue() {}
  
  // blocking push until timeout or push sucessfully
  virtual void pushUntil(ParamType ele, TimeUnit time = 0) = 0;
  
  // blocking pop until timeout or pop sucessfully
  // User should check the return value is valid or not
  virtual ElementType popUntil(TimeUnit time = 0) = 0;
  
  // this is uncopy version of popUtil
  // if return false, these mush be some exceptions happened
  virtual bool popUntil(ElementType& ele, TimeUnit time = 0) = 0;
  
  
  // unblocking pop and push
  virtual void push(ParamType ele) = 0;
  virtual ElementType pop() = 0;
  virtual bool pop(ElementType& ele) = 0;
  
  // other utility functions
  virtual size_t size() const = 0;
  virtual bool isEmpty() const= 0;
  
  // XXX : should I support front operation ??
};

#endif
