#ifndef LINKEDBLOCKINGQUEUE_H_
#define LINKEDBLOCKINGQUEUE_H_

#include <list>
#include "BlockingQueue.h"
#include "Locks.h"

#define DEFAULT_MAX_CAPABILITY INT32_MAX
template <typename T>
class LinkedBlockingQueue : public BlockingQueue<T>
{
public: 
    explicit LinkedBlockingQueue(unsigned int capability)
        : m_capability(capability)
    {
    }

    virtual void pushUntil(ParamType ele, TimeUnit time = 0)
    {
    }

    virtual ElementType popUntil(TimeUnit time = 0)
    {
    }

    virtual bool popUntil(ElementType& ele, TimeUnit time = 0)
    {
        return true;
    }

    virtual void push(ParamType ele)
    {
    }

    virtual ElementType pop()
    {
    }

    virtual bool pop(ElementType& ele)
    {
        return false;
    }

    virtual size_t size() const
    {
        return 0;
    }

    virtual bool isEmpty() const
    {
        return true;
    }

    virtual bool remove(ParamType ele, EqualFunc func = EqualFunc())
    {
        MutexLockGuard lock1(m_frontLock);
        MutexLockGuard lock2(m_backLock);
        return true;
    }
private:
    // pop front : front lock and empty condition
    MutexLock m_frontLock;
    Condition m_emptyCond;

    // push back : back lock and fully condition
    MutexLock m_backLock;
    Condition m_fullCond;

    // element type ??
    std::list<ElementType> m_elements;
    const unsigned long int m_capability;
};
#endif  // LINKEDBLOCKINGQUEUE_H_

