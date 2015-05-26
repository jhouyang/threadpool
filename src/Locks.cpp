#include "Locks.h"
#include "ThisThread.h"
#include <sys/syscall.h>
#include <sys/time.h>

#include <stdlib.h>

namespace {
    inline void toTimeSpec(struct timespec& timeout, TimeUnit ms) {
        struct timeval now;
        
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + static_cast<long>(ms / 1000);
        timeout.tv_nsec = now.tv_usec + static_cast<long>(ms % 1000 *1000);
        
        // for safety
        if (timeout.tv_nsec >= 1000000) {
            ++timeout.tv_sec;
            timeout.tv_nsec = timeout.tv_nsec % 1000000;
        }
        timeout.tv_nsec *= 1000;
    }
}  // anonymous namespace

#ifndef assert
#define assert(x) \
    do { \
        if (!(x)) { \
            abort(); \
        } \
    } while(0)
#endif

MutexLock::MutexLock(const pthread_mutexattr_t& attr)
    : holder_(0)
{
    pthread_mutex_init(&mutex_, &attr);
}

MutexLock::MutexLock()
    : holder_(0)
{
    pthread_mutex_init(&mutex_, NULL);
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

void Condition::timedWait(TimeUnit ms)
{
    struct timespec timeout;
    ::toTimeSpec(timeout, ms);
    
    int ret = pthread_cond_timedwait(&cond_, mutex_.getMutex(), &timeout);
    // TODO : how to deal with ret ?
    // refer to http://linux.die.net/man/3/pthread_cond_timedwait
    // it will only return failure : EINVAL EPERM
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


