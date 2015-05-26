#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include "boost/noncopyable.hpp"
#include "boost/function.hpp"

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
public:
    typedef T ElementType;
    typedef typename ConstRefTrait<T>::ConRefType ParamType;
    typedef typename ConstRefTrait<T>::RefType RefType;

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

    // remove function, if user doesn't pass the EqualFunc,
    // will use ElementType == ElementType to compare
    typedef boost::function< bool (ParamType, ParamType) > EqualFunc;
    virtual bool remove(ParamType ele, EqualFunc func = EqualFunc()) = 0;
};

#endif
