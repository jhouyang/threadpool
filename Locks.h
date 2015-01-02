#include <pthread.h>

typedef pthread_mutex_t MutexType;
#define MUTEX_INIT_VAL PTHREAD_MUTEX_INITIALIZER 

MutexType taskQueueMutex = MUTEX_INIT_VAL;

class MutexLockBlock
{
public:
    MutexLockBlock()
    {
        pthread_mutex_lock(&taskQueueMutex);
    }
    ~MutexLockBlock()
    {
        pthread_mutex_unlock(&taskQueueMutex);
    }
};
