#ifndef THREADPOOL_TASKSQUEUE_H_
#define THREADPOOL_TASKSQUEUE_H_

#include <list>
#include <pthread.h>
#include <boost/function.hpp>

class TaskBase
{
public:
    virtual ~TaskBase() { }
    virtual void Run() = 0;
    // virtual bool IsTaskFinished() const = 0;
};

typedef boost::function< void(void) > FuncType;
class FuncTask : public TaskBase
{
public:
    // you can use boost::bind to get function you want
    FuncTask(const FuncType& func);
    void Run();
private:
    FuncType m_func;
};

typedef boost::function< int (TaskBase*, TasksBase*) > SortFunc;
class TasksQueueBase
{
public:
    virtual ~TasksQueue() {}
    
    // should delete the TaskBase to avoid memory leak by the user ocde
    virtual TaskBase* PopTask() = 0;
    
    // TasksQueueBase will own it lifetime
    void PushTask(TaskBase* task) = 0;
    
    // sort tasks
    // no need to support Sort function for all tasksQueue
    void Sort(const SortFunc& func) {};
};

class TasksQueue
{
public:
    TasksQueue();
    ~TasksQueue();

    void SetCancel(bool bCancel = true);

    // could be used by threads
    TaskBase* PopTask();
    void PushTask(TaskBase* task);

private:
    std::list<TaskBase*> m_tasks;
    unsigned int m_waitThreads;
    bool m_bCancel : 1;

    pthread_cond_t m_getTasksCond;
    pthread_mutex_t m_mutex;
};

#endif  // THREADPOOL_TASKSQUEUE_H_

