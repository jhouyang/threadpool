#include "TasksTasksQueue.h"
#include "Locks.h"

TasksQueue::TasksQueue()
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
}

TaskBase* TasksQueue::PopTask()
{
    TaskBase* ptr = NULL;
    {
        MutexLockBlock mutex_;
        if (m_tasks.empty())
        {
            return ptr;
        }
        ptr = m_tasks.front();
        m_tasks.pop_front();
        return ptr;
    }
}

bool TasksQueue::IsEmpty() const
{
    {
        MutexLockBlock mutex_;
        return m_tasks.size() == 0;
    }
}

