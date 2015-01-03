#include "TasksQueue.h"
#include "Locks.h"

pthread_mutex_t TasksQueue::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t TasksQueue::m_cond = PTHREAD_COND_INITIALIZER;

TasksQueue::TasksQueue()
    : m_bHasWaitTask(false)
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
    if (m_bHasWaitTask)
        pthread_cond_broadcast(&TasksQueue::m_cond);
}

TaskBase* TasksQueue::PopTask()
{
    TaskBase* ptr = NULL;
    {
        MutexLockBlock mutex_(&TasksQueue::m_mutex);
        while (m_tasks.empty())
        {
            m_bHasWaitTask = true;
            pthread_cond_wait(&TasksQueue::m_cond, &TasksQueue::m_mutex);
        }
        ptr = m_tasks.front();
        m_tasks.pop_front();
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

