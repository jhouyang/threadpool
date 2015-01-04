#include "thread.h"
#include "Locks.h"
#include <assert.h>

ThreadBase::ThreadBase(bool bDetached)
    : m_isDetached(bDetached)
    , m_isStarted(false)
    , m_isDestroyed(false)
    , m_isPaused(false)
{
    pthread_mutex_init(&m_mutex, NULL);

    // work just like pthread_cond_t
    sem_init(&m_semStart, 0, 0);
    sem_init(&m_semPause, 0, 0);
}

ThreadBase::~ThreadBase()
{
    pthread_mutex_destroy(&m_mutex);
}

bool ThreadBase::IsDetached() const
{
    return m_isDetached;
}

void ThreadBase::Create()
{
    MutexLockBlock mutex_(m_mutex);
    DoCreate_unlocked();
}

void ThreadBase::Start()
{
    MutexLockBlock mutex_(m_mutex);
    if (!m_isStarted)
    {
        DoCreate_unlocked();
    }
    SignalStart();
}

void ThreadBase::DoCreate_unlocked()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (m_isDetached && 
        0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
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

void ThreadBase::WaitStart()
{
    sem_wait(&m_semStart);
}

void ThreadBase::SignalStart()
{
    sem_post(&m_semStart);
}

pthread_mutex_t* ThreadBase::GetMutex() const
{
    return *m_mutex;
}

void* ThreadBase::_threadFunc(void* data)
{
    ThreadBase* pthread = static_cast<ThreadBase*>(data);
    if (!pthread) return;

    // wait for start signal
    pthread->WaitStart();

    {
        MutexLockBlock mutex_(pthread->GetMutex());
        if (pthread->IsDestroyed())
        {
            // FIXME: testpoint: will it work ? test it;
            delete pthread;
        }
    }
    pthread->Entry();
    {
        MutexLockBlock mutex_(pthread->GetMutex());
        pthread->SetDestroy();
    }
}

/* TODO:
    void SetDestroy();
TODO : SetDestroy should wake up thread : two kinds of sem_wait
    bool IsDestroyed() const;

    // real thread main loop should call this function to really stop the thread
    bool CheckDestroy() const;
*/


// TODO list
// fix me : m_destroyed/ m_cancelled, two concept
// use ThreadStat enum

