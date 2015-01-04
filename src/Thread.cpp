#include "thread.h"
#include "Locks.h"
#include <assert.h>

ThreadBase::ThreadBase(bool bDetached)
    : m_isDetached(bDetached)
    , m_isStarted(false)
    , m_isDestroyed(false)
    , m_isPaused(false)
{
    m_mutex = PTHREAD_MUTEX_INITIALIZER;

    // work just like pthread_cond_t
    sem_init(&m_semStart, 0, 0);
    sem_init(&m_semPause, 0, 0);
}

ThreadBase::~ThreadBase()
{
}

void ThreadBase::IsDetached() const
{
    return m_isDetached;
}

void ThreadBase::Create()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (m_isDetached && 
        0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DEATCHED))
    {
        // FIXME : should add log system to info this assert
        assert(0);
    }

    int retVal = pthread_create(&m_threadID, &attr, _threadFunc, this);

    if (pthread_attr_destroy(&attr) != 0)
    {
        // FIXME : log system
        assert(0);
    }
    if (retVal != 0) // pthread_create failed
    {
        // thread should be destroyed
        // FIXME : maybe should throw exception here to let outside code know about this
        m_isDestroyed = true;
    }

    m_isStarted = true;
}

void ThreadBase::StartWait()
{
    sem_wait(&m_semStart);
}

void ThreadBase::SignalStart()
{
    sem_post(&m_semStart);
}

