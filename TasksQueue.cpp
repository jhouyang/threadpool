#include "TasksQueue.h"
#include "Locks.h"

Queue::Queue()
{
}

Queue::~Queue()
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

void Queue::PushTask(TaskBase* task)
{
    m_tasks.push_back(task);
}

TaskBase* Queue::PopTasks()
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

bool Queue::IsEmpty() const
{
    {
        MutexLockBlock mutex_;
        return m_tasks.size() == 0;
    }
}

