#include "TasksQueue.h"
#include "Locks.h"
#include "assert.h"

pthread_mutex_t TasksQueue::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TasksQueue::m_cond = PTHREAD_COND_INITIALIZER;

TasksQueue::TasksQueue()
    : m_waitThreads(0)
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
    m_tasks.push_back(task);
    if (m_waitThreads > 0)
        pthread_cond_broadcast(&TasksQueue::m_cond);
}

TaskBase* TasksQueue::PopTask()
{
    TaskBase* ptr = NULL;
    {
        MutexLockBlock mutex_(&TasksQueue::m_mutex);
        // maybe we could use if here, 
        // cause the mutex will make sure it be called only by one thread at a time
        while (m_tasks.empty() || m_bCancel)
        {
            ++m_waitThreads;
            pthread_cond_wait(&TasksQueue::m_cond, &TasksQueue::m_mutex);
        }
        ptr = m_tasks.front();
        m_tasks.pop_front();

        // outside call PopTask to execute task, waitThreads--
        assert(m_waitThreads > 0);
        --m_waitThreads;
        return ptr;
    }
}

bool TasksQueue::IsEmpty() const
{
    {
        MutexLockBlock mutex_(&TasksQueue::m_mutex);
        return m_tasks.size() == 0;
    }
}

void TasksQueue::SetCancel(bCancel /*= true*/)
{
    m_bCancel = bCancel;
}

