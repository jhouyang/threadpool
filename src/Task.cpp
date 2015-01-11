#include "Task.h"
#include "Locks.h"
#include "assert.h"

namespace
{
    struct CancelTaskException {};
    struct UnexpectedTaskRunException {};
}  // anonymous namespace

/*****************************Start of CancellableTask*************************************/
CancellableTask::CancellableTask()
    : m_needCancel(false)
    , m_state(STAT_INIT)
{
    pthread_mutex_init(&m_cancelMutex, NULL);
    pthread_mutex_init(&m_statMutex, NULL);
    pthread_cond_init(&m_waitStatCond, NULL);
}

CancellableTask::~CancellableTask()
{
    pthread_mutex_destroy(&m_statMutex);
    pthread_mutex_destroy(&m_cancelMutex);
    pthread_cond_destroy(&m_waitStatCond);
}

void CancellableTask::SetState(CancellableTaskState state)
{
    MutexLockBlock mutex_(m_statMutex);
    m_state = state;
}

void CancellableTask::Run()
{
    bool taskFinishedUnexpected = false;
    try
    {
        CheckCancellation();
        SetState(STAT_RUNNING);
        
        DoRun();
        
        SetState(STAT_FINISHED);
    }
    catch (const CancelTaskException&)
    {
        SetState(STAT_CANCELLED);
    }
    catch (...)
    {
        taskFinishedUnexpected = true;
        SetState(STAT_FINISHED);
    }

    pthread_cond_signal(&m_waitStatCond);
    
    if (taskFinishedUnexpected)
    {
        throw UnexpectedTaskRunException();
    }
}

void CancellableTask::Cancel()
{
    MutexLockBlock mutex_(m_cancelMutex);
    m_needCancel = true;
}

void CancellableTask::CancelWait()
{
    Cancel();

    MutexLockBlock mutex_(m_statMutex);
    while (m_state != STAT_CANCELLED && m_state != STAT_FINISHED)
    {
        pthread_cond_wait(&m_waitStatCond, &m_statMutex);
    }
}
    
CancellableTask::CancellableTaskState CancellableTask::GetState()
{
    MutexLockBlock mutex_(m_statMutex);
    return m_state;
}

void CancellableTask::CheckCancellation()
{
    MutexLockBlock mutex_(m_cancelMutex);
    if (true == m_needCancel)
    {
        throw CancelTaskException();
    }
}

/*****************************End of CancellableTask*************************************/


FuncTask::FuncTask(const FuncType& func)
    : m_func(func)
{
}

void FuncTask::Run()
{
    m_func();
}

