#include "Task.h"
#include "Locks.h"
#include "assert.h"

namespace
{
    struct CancelTaskException {};
}  // anonymous namespace

/*****************************Start of CancellableTask*************************************/
CancellableTask::CancellableTask()
    : m_needCancel(false)
    , m_state(STAT_INIT)
    , m_statMutex()
    , m_cancelCond(m_cancelMutex)

{
}

CancellableTask::~CancellableTask()
{
}

void CancellableTask::SetState(CancellableTaskState state)
{
    MutexLockGuard mutex_(m_statMutex);
    m_state = state;
}

void CancellableTask::Run()
{
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

    m_cancelCond.notifyAll();
}

void CancellableTask::Cancel()
{
    MutexLockGuard mutex_(m_cancelMutex);
    m_needCancel = true;
}

void CancellableTask::CancelWait()
{
    Cancel();

    MutexLockGuard mutex_(m_cancelMutex);
    while (m_state != STAT_CANCELLED && m_state != STAT_FINISHED)
    {
        m_cancelCond.wait();
    }
}
    
CancellableTask::CancellableTaskState CancellableTask::GetState()
{
    MutexLockGuard mutex_(m_statMutex);
    return m_state;
}

void CancellableTask::CheckCancellation()
{
    MutexLockGuard mutex_(m_cancelMutex);
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

