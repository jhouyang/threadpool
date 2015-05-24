#ifndef THREADPOOL_LOCKS_H_
#define THREADPOOL_LOCKS_H_

#include <pthread.h>
#include <unistd.h>

#include <iostream>

class Condition;
class MutexLockGuard;
class MutexLock
{
public:
    friend class Condition;
    friend class MutexLockGuard;

    MutexLock(const pthread_mutexattr_t& attr);
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
    pthread_mutexattr_t attr_;
    pthread_t holder_;
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
private:
    Condition(const Condition&);
    Condition& operator=(const Condition&);
private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
};

void printids(std::ostream& os);

#endif  // THREADPOOL_LOCKS_H_

