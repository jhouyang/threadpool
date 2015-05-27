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
    
    int getAndIncre()
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val++;
        }
    }
    
    int getAndDecre()
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val--;
        }
    }
    
    in getAndSet(int val)
    {
        {
            MutexLockGuard lock(m_lock);
            int tmp = m_val;
            m_val = val;
            return tmp;
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

/*
typedef struct { volatile int counter; } atomic_t;
#define automic_set(v, i) (((v)->counter) = (i)) 
static __inline__ void atomic_add(int i, atomic_t *v)
{
__asm__ __volatile__(
LOCK "addl %1,%0"
:"=m" (v->counter)
:"ir" (i), "m" (v->counter));
} 
*/
#endif  // ATOMIC_H_
