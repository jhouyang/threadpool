#ifndef ATOMIC_H_
#define ATOMIC_H_

#include <boost/noncopyable.hpp>
// TODO : use assemble to implement atomic
// TODO : use template to apply to more types
// TODO : support copy constructor and operator =
// TODO : other operations

// support unsigned int only
class AtomicUInt : boost::noncopyable
{
public:
    explicit AtomicUInt(unsigned int val)
        : m_val(val)
    {
    }
    
    unsigned int get() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val;
        }
    }
    
    unsigned int getAndIncre() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val++;
        }
    }
    
    unsigned int getAndDecre() const
    {
        {
            MutexLockGuard lock(m_lock);
            return m_val--;
        }
    }
    
    bool equal(unsigned int val) const
    {
        {
            MutexLockGuard lock(m_lock);
            return val == m_val;
        }
    }
    
private:
    mutable MutexLock m_lock;
    unsigned int m_val;
};

#endif  // ATOMIC_H_
