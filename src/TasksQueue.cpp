#include "TasksQueue.h"
#include "Locks.h"
#include "assert.h"

pthread_mutex_t TasksQueue::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TasksQueue::m_cond = PTHREAD_COND_INITIALIZER;

TasksQueue::TasksQueue()
    : m_waitThreads(0)
    , m_bCancel(false)
{
}

TasksQueue::~TasksQueue()
{
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
        MutexLockBlock mutex_(&TasksQueue::m_mutex);
        m_tasks.push_back(task);
    }
    if (m_waitThreads > 0)
    {
        pthread_cond_broadcast(&TasksQueue::m_cond);
    }
}

TaskBase* TasksQueue::PopTask()
{
    MutexLockBlock mutex_(&TasksQueue::m_mutex);

    // maybe we could use if here, 
    // cause the mutex will make sure it be called only by one thread at a time
    // and pthread_cond_wait will try to lock the mutex before return
    while (m_tasks.empty() || m_bCancel)
    {
        // FIXME : while block may have problem
        ++m_waitThreads;
        pthread_cond_wait(&TasksQueue::m_cond, &TasksQueue::m_mutex);
    }

    TaskBase* ptr = m_tasks.front();
    m_tasks.pop_front();

    // outside call PopTask to execute task, waitThreads--
    assert(m_waitThreads > 0);
    --m_waitThreads;
    return ptr;
}

void TasksQueue::SetCancel(bool bCancel /*= true*/)
{
    if (m_bCancel == bCancel) return;
    m_bCancel = bCancel;

    // resume
    if (!m_bCancel)
    {
        pthread_cond_broadcast(&TasksQueue::m_cond);
    }
}

