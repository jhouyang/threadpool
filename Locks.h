#include <pthread.h>
#include <unistd.h>

#include <iostream>

class MutexLockBlock
{
public:
    MutexLockBlock(pthread_mutex_t* mutex)
        : m_mutex(mutex)
    {
        pthread_mutex_lock(m_mutex);
    }
    ~MutexLockBlock()
    {
        pthread_mutex_unlock(m_mutex);
    }
private:
    pthread_mutex_t* m_mutex;
};

void printids(std::ostream& os)
{
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    
    os << "process ID is " << pid << "\t" << "thread ID is " << tid << "\n";
}
