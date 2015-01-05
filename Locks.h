#ifndef THREADPOOL_LOCKS_H_
#define THREADPOOL_LOCKS_H_

#include <pthread.h>
#include <unistd.h>

#include <iostream>

class MutexLockBlock
{
public:
    MutexLockBlock(pthread_mutex_t* mutex);
    ~MutexLockBlock();
private:
    pthread_mutex_t* m_mutex;
};

void printids(std::ostream& os);

#endif  // THREADPOOL_LOCKS_H_

