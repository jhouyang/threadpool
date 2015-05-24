#include "TasksQueue.h"
#include "Locks.h"
#include "assert.h"
#include "Task.h"

/*****************************Start of TasksQueue*************************************/
BlockingTasksQueue::BlockingTasksQueue()
    : m_waitThreads(0)
    , m_bCancel(false)
    , m_mutex()
    , m_getTasksCond(m_mutex)
{
}

BlockingTasksQueue::~BlockingTasksQueue()
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

void BlockingTasksQueue::PushTask(TaskBase* task)
{
    {
        MutexLockGuard mutex_(m_mutex);
        m_tasks.push_back(task);
    }
    if (m_waitThreads > 0)
    {
        m_getTasksCond.notifyAll();
    }
}

TaskBase* BlockingTasksQueue::PopTask()
{
    MutexLockGuard mutex_(m_mutex);

    // we can't use if here, 
    // cause broadcast may wakeup more than one waiting thread
    ++m_waitThreads;
    while (m_tasks.empty() || m_bCancel)
    {
        m_getTasksCond.wait();
    }
    --m_waitThreads;

    TaskBase* ptr = m_tasks.front();
    m_tasks.pop_front();

    // outside call PopTask to execute task, waitThreads--
    return ptr;
}

void BlockingTasksQueue::Pause()
{
    MutexLockGuard mutex_(m_mutex);
    if (true == m_bCancel) return;
    m_bCancel = true;
}

void BlockingTasksQueue::Resume()
{
    {
        MutexLockGuard mutex_(m_mutex);
        if (false == m_bCancel) return;
        m_bCancel = false;
    }
    m_getTasksCond.notifyAll();
}
/*****************************End of TasksQueue*************************************/

