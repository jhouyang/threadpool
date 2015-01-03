#include <pthread.h>

class MutexLockBlock
{
public:
    MutexLockBlock(pthread_mutex_t* mutex)
        : m_mutex(mutex)
    {
        pthread_mutex_lock(m_mutex);
        pthread_t tid = pthread_self();
        printf("Lock %p\n", tid);
    }
    ~MutexLockBlock()
    {
        pthread_mutex_unlock(m_mutex);
    }
private:
    pthread_mutex_t* m_mutex;
};
