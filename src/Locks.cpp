#include "Locks.h"

#ifndef assert
#define assert(x) \
    do { \
        if (!(x)) { \
            abort(); \
        } \
    } while(0)
#endif

class ThisThread
{
public:
    static pthread_t tid()
    {
        return pthread_self();
    }
};

MutexLock::MutexLock(const pthread_mutexattr_t& attr)
    : holder_(0)
    // own a copy of pthread_mutex_attr
        // TODO : pthread_mutex_attr
    , attr_(attr)
{
    pthread_mutex_init(&mutex_, &attr);
}

MutexLock::~MutexLock()
{
    assert(holder_ == 0);
    pthread_mutex_destroy(&mutex_);
}

bool MutexLock::isLockedByThisThread()
{
    return holder_ == ThisThread::tid();
}

void MutexLock::assertLocked()
{
    assert(isLockedByThisThread());
}

void MutexLock::lock()
{
    pthread_mutex_lock(&mutex_);
    holder_ = ThisThread::tid();
}

void MutexLock::unlock()
{
    holder_ = 0;
    pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* MutexLock::getMutex()
{
    return &mutex_;
}

MutexLockGuard::MutexLockGuard(MutexLock& mutex)
    : mutex_(mutex)
{
    mutex_.lock();
}
MutexLockGuard::~MutexLockGuard()
{
    mutex_.unlock();
}

Condition::Condition(MutexLock& mutex)
    : mutex_(mutex)
{
    pthread_cond_init(&cond_, NULL);
}

Condition::~Condition()
{
    pthread_cond_destroy(&cond_);
}

void Condition::wait()
{
    pthread_cond_wait(&cond_, mutex_.getMutex());
}

void Condition::notify()
{
    pthread_cond_signal(&cond_);
}

void Condition::notifyAll()
{
    pthread_cond_broadcast(&cond_);
}

void printids(std::ostream& os)
{
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    
    os << "process ID is " << pid << "\t" << "thread ID is " << tid << "\n";
}


