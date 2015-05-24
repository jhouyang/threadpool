#ifndef THREADPOOL_TASKSQUEUE_H_
#define THREADPOOL_TASKSQUEUE_H_

#include <list>
#include <pthread.h>
#include <Locks.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

class TaskBase;

typedef boost::function< int (TaskBase*, TaskBase*) > SortFunc;
class TasksQueueBase
{
public:
    typedef boost::shared_ptr<TasksQueueBase> TasksQueueBasePtr;
    virtual ~TasksQueueBase() {}
    
    // should delete the TaskBase to avoid memory leak by the user ocde
    virtual TaskBase* PopTask() = 0;
    
    // TasksQueueBase will own it lifetime
    virtual void PushTask(TaskBase* task) = 0;
    
    // sort tasks
    // no need to support Sort function for all tasksQueue
    void Sort(const SortFunc& func) {};
};

class BlockingTasksQueue : public TasksQueueBase
{
public:
    typedef boost::shared_ptr<BlockingTasksQueue> BlockingTasksQueuePtr;
    BlockingTasksQueue();
    ~BlockingTasksQueue();

    void Pause();
    void Resume();

    // could be used by threads
    TaskBase* PopTask();
    void PushTask(TaskBase* task);

private:
    std::list<TaskBase*> m_tasks;
    unsigned int m_waitThreads;
    bool m_bCancel : 1;

    MutexLock m_mutex;
    Condition m_getTasksCond;
};

typedef BlockingTasksQueue TasksQueue;

#endif  // THREADPOOL_TASKSQUEUE_H_

