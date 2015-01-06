#include "TasksQueue.h"
#include "Locks.h"
#include "assert.h"

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

