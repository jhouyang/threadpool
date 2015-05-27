#ifndef LINKEDBLOCKINGQUEUE_H_
#define LINKEDBLOCKINGQUEUE_H_

#include <list>
#include <boost/typeof/typeof.hpp>

#include "BlockingQueue.h"
#include "Locks.h"
#include "Atomic.h"

#define DEFAULT_MAX_CAPABILITY INT16_MAX

template <typename T>
class LinkedBlockingQueue : public BlockingQueue<T>
{
    #define BEGIN_FULLY_LOCK \
        MutexLockGuard putLock(m_putLock); \
        { \
            MutexLockGuard popLock(m_popLock);
    #define END_FULLY_LOCK \
        }
public: 
    explicit LinkedBlockingQueue(int capability)
        : m_capability(capability), m_count(0)
    {
        assert(capability > 0);
    }

    virtual bool pushUntil(ParamType ele, TimeUnit time = 0) {
        // nonblocking push
        if (time == 0 && m_count.equal(m_capability)) {
            return false;
        }

        int oldCount = -1;
        {
            MutexLockGuard putLock(m_putLock);
            while (m_count.get() == m_capability) {
                if (m_fullCond.timedWait(time) < 0 ) {
                    return false;
                }
            }
        
            oldCount = doPushAndNofity_unlock(ele);
        }

        if (oldCount == 0) {
            notifyNotEmpty();
        }
        return true;
    }

    virtual bool popUntil(RefType ele, TimeUnit time = 0) {
        // nonblocking pop
        if (time == 0 && m_count.equal(0)) {
            return false;
        }
        int oldCount = -1;
        {
            MutexLockGuard popLock(m_popLock);
            while (m_count.get() == 0) {
                if (m_emptyCond.timedWait(time) < 0 ) {
                    return false;
                }
            }
        
            oldCount = doPopAndNotify_unlock(ele);
        }

        if (oldCount == m_capcability) {
            notifyNotFull();
        }
        return true;
    }

    virtual void push(ParamType ele) {
        int oldCount = -1;
        {
            MutexLockGuard putLock(m_putLock);
            while (m_count.get() == m_capability) {
                m_fullCond.wait();
            }
        
            oldCount = doPushAndNofity_unlock(ele);
        }
        
        if (oldCount == 0) {
            notifyNotEmpty();
        }
    }

    virtual ElementType pop() {
        int oldCount = -1;
        ElementType ele;
        {
            MutexLockGuard takeLock(m_popLock);
            while (m_count.get() == 0) {
                m_emptyCond.wait();
            }
            
            oldCount = doPopAndNotify_unlock(ele);
        }
        
        if (oldCount == m_capability) {
            notifyNotFull();
        }
        
        return ele;
    }

    virtual void pop(RefType ele) {
        int oldCount = -1;
        {
            MutexLockGuard takeLock(m_popLock);
            while (m_count.get() == 0) {
                m_emptyCond.wait();
            }
            
            oldCount = doPopAndNotify_unlock(ele);
        }
        
        if (oldCount == m_capability) {
            notifyNotFull();
        }
    }

    virtual size_t size() const {
        return m_count.get();
    }

    virtual bool isEmpty() const {
        return m_count.equal(0);
    }

    virtual bool remove(ParamType ele, EqualFunc func = EqualFunc()) {
        BEGIN_FULLY_LOCK
        BOOST_AUTO(it, m_list.begin());
        BOOST_AUTO(itEnd, m_list.end());

        bool equal = false;
        while (it != itEnd) {
            if (func) {
                equal = func(ele, *it);
            }
            else {
                equal = (ele == *it);
            }
            
            if (equal) {
                m_list.erase(it);
                return true;
            }
            
            ++it;
        }
        
        return false;
        END_FULLY_LOCK
    }
private:
    void notifyNotEmpty() const {
        MutexLockGuard popLock(m_popLock);
        m_emptyCond.notify();
    }
    
    void notifyNotFull() const {
        MutexLockGuard putLock(m_putLock);
        m_fullCond.notify();
    }
    
    int doPushAndNofity_unlock(ParamType ele) {
        m_list.push_back(ele);
        int oldCount = m_count.getAndIncre();
        if (oldCount + 1 < m_capability) {
            m_fullCond.notify();
        }
        return oldCount;
    }
    
    int doPopAndNotify_unlock(RefType ele) {
        ele = m_list.front();
        m_list.pop_front();

        int oldCount = m_count.getAndDecre();
        if (oldCount > 1) {
            m_emptyCond.notify();
        }
        return oldCount;
    }
private:
    // pop front : front lock and empty condition
    mutable MutexLock m_popLock;
    mutable Condition m_emptyCond;

    // push back : back lock and fully condition
    mutable MutexLock m_putLock;
    mutable Condition m_fullCond;

    AtomicInt m_count;
    
    std::list<ELementType> m_list;

    const int m_capability;
};
#endif  // LINKEDBLOCKINGQUEUE_H_

