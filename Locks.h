#ifndef THREADPOOL_LOCKS_H_
#define THREADPOOL_LOCKS_H_

#include <pthread.h>
#include <unistd.h>

typedef unsigned long TimeUnit;

class Condition;
class MutexLockGuard;
class MutexLock
{
public:
    friend class Condition;
    friend class MutexLockGuard;

    explicit MutexLock(const pthread_mutexattr_t& attr);
    MutexLock();
    ~MutexLock();

    bool isLockedByThisThread();
    void assertLocked();

private:
    // no copyable
    MutexLock(const MutexLock&);
    MutexLock& operator=(const MutexLock&);

private:
    // only MutexLockGuard can acess functions below
    void lock();
    void unlock();

    pthread_mutex_t* getMutex();
private:
    pthread_mutex_t mutex_;
    int holder_;
};

class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock& mutex);
    ~MutexLockGuard();
private:
    MutexLock& mutex_;
};

class Condition
{
public:
    Condition(MutexLock& mutex);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();
    
    void timedWait(TimeUnit seconds);
private:
    Condition(const Condition&);
    Condition& operator=(const Condition&);
private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
};

#endif  // THREADPOOL_LOCKS_H_

