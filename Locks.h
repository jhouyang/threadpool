#include <pthread.h>

class MutexLockBlock
{
public:
    MutexLockBlock(pthread_mutex_t* mutex)
        : m_mutex(mutex)
    {
        printf("Lock %p\n", m_mutex);
        pthread_mutex_lock(m_mutex);
    }
    ~MutexLockBlock()
    {
        pthread_mutex_unlock(m_mutex);
    }
private:
    pthread_mutex_t* m_mutex;
};
