#ifndef LINKEDBLOCKINGQUEUE_H_
#define LINKEDBLOCKINGQUEUE_H_

#include "BlockingQueue.h"
#include "Locks.h"

#define DEFAULT_MAX_CAPABILITY INT16_MAX
template <typename T>
class LinkedBlockingQueue : public BlockingQueue<T>
{
public: 
    virtual void pushUntil(ParamType ele, TimeUnit time = 0);
    virtual ElementType popUntil(TimeUnit time = 0);
    virtual bool popUntil(ElementType& ele, TimeUnit time = 0);

    virtual void push(ParamType ele);
    virtual ElementType pop();
    virtual bool pop(ElementType& ele);

    virtual size_t size() const;
    virtual bool isEmpty() const;

    virtual bool remove(ParamType ele, EqualFunc func = EqualFunc());
private:
    void fullyLock();
    void fullyUnLock;
private:
    // pop front : front lock and empty condition
    MutexLock m_frontLock;
    Condition m_emptyCond;

    // push back : back lock and fully condition
    MutexLock m_backLock;
    Condition m_fullCond;
};
#endif  // LINKEDBLOCKINGQUEUE_H_

