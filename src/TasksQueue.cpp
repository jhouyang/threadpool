#include "TasksQueue.h"
#include "Locks.h"
#include "assert.h"
#include "Task.h"

/*****************************Start of TasksQueue*************************************/
BlockingTasksQueue::BlockingTasksQueue()
    : m_waitThreads(0)
    , m_bCancel(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_getTasksCond, NULL);
}

BlockingTasksQueue::~BlockingTasksQueue()
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

void BlockingTasksQueue::PushTask(TaskBase* task)
{
    {
        MutexLockBlock mutex_(m_mutex);
        m_tasks.push_back(task);
    }
    if (m_waitThreads > 0)
    {
        pthread_cond_broadcast(&m_getTasksCond);
    }
}

TaskBase* BlockingTasksQueue::PopTask()
{
    MutexLockBlock mutex_(m_mutex);

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

void BlockingTasksQueue::Pause()
{
    MutexLockBlock mutex_(m_mutex);
    if (true == m_bCancel) return;
    m_bCancel = true;
}

void BlockingTasksQueue::Resume()
{
    {
        MutexLockBlock mutex_(m_mutex);
        if (false == m_bCancel) return;
        m_bCancel = false;
    }
    pthread_cond_broadcast(&m_getTasksCond);
}
/*****************************End of TasksQueue*************************************/

