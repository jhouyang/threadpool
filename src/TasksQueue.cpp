#include "TasksQueue.h"
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
    MutexLockBlock mutex_(&m_statMutex);
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
        SetState(STAT_CANCELLED):
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
    MutexLockBlock mutex_(&m_cancelMutex);
    m_needCancel = true;
}

void CancellableTask::CancelWait()
{
    Cancel();

    MutexLockBlock mutex_(&m_statMutex);
    while (m_state != STAT_CANCELLED && m_state != STAT_FINISHED)
    {
        pthread_cond_wait(&m_waitStatCond, &m_statMutex);
    }
}
    
CancellableTaskState CancellableTask::GetState() const
{
    MutexLockBlock mutex_(&m_statMutex);
    return m_state;
}

void CancellableTask::CheckCancellation()
{
    MutexLockBlock mutex_(&m_cancelMutex);
    if (true == m_needCancel)
    {
        throw CancelTaskException();
    }
}

/*****************************End of CancellableTask*************************************/


/*****************************Start of TasksQueue*************************************/
TasksQueue::TasksQueue()
    : m_waitThreads(0)
    , m_bCancel(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_getTasksCond, NULL);
}

TasksQueue::~TasksQueue()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_getTasksCond);
    if (!m_tasks.empty())
    {
        std::list<TaskBase*>::iterator it = m_tasks.begin();
        for (; it != m_tasks.end(); )
        {
            TaskBase* tmp = *it;
            it = m_tasks.erase(it);
            delete tmp;
        }
    }
}

void TasksQueue::PushTask(TaskBase* task)
{
    {
        MutexLockBlock mutex_(&m_mutex);
        m_tasks.push_back(task);
    }
    if (m_waitThreads > 0)
    {
        pthread_cond_broadcast(&m_getTasksCond);
    }
}

TaskBase* TasksQueue::PopTask()
{
    MutexLockBlock mutex_(&m_mutex);

    // we can't use if here, 
    // cause broadcast may wakeup more than one waiting thread
    ++m_waitThreads;
    while (m_tasks.empty() || m_bCancel)
    {
        pthread_cond_wait(&m_getTasksCond, &m_mutex);
    }
    --m_waitThreads;

    TaskBase* ptr = m_tasks.front();
    m_tasks.pop_front();

    // outside call PopTask to execute task, waitThreads--
    return ptr;
}

void TasksQueue::SetCancel(bool bCancel /*= true*/)
{
    {
        MutexLockBlock mutex_(&m_mutex);
        if (m_bCancel == bCancel) return;
        m_bCancel = bCancel;
        if (m_bCancel) return;
    }

    pthread_cond_broadcast(&m_getTasksCond);
}
/*****************************End of TasksQueue*************************************/

FuncTask::FuncTask(const FuncType& func)
    : m_func(func)
{
}

void FuncTask::Run()
{
    m_func();
}
