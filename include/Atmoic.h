#ifndef ATOMIC_H_
#define ATOMIC_H_

#include "Locks.h"
#include <boost/noncopyable.hpp>
// TODO : use assemble to implement atomic
// TODO : use template to apply to more types
// TODO : support copy constructor and operator =
// TODO : other operations

// support int only
class AtomicInt : boost::noncopyable
{
public:
    explicit AtomicInt(int val)
        : m_val(val)
    {
    }
    
    int get() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val;
        }
    }
    
    int getAndIncre() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val++;
        }
    }
    
    int getAndDecre() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val--;
        }
    }
    
    bool equal(int val) const
    {
        {
            MutexLockGuard lock(m_lock);
            return val == m_val;
        }
    }
    
private:
    mutable MutexLock m_lock;
    int m_val;
};

#endif  // ATOMIC_H_
