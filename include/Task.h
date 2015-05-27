#ifndef THREADPOOL_TASK_H_
#define THREADPOOL_TASK_H_

#include <list>
#include <pthread.h>
#include "Locks.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

class TaskBase
{
public:
    typedef boost::shared_ptr<TaskBase> TaskBasePtr;
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

class CancellableTask : public TaskBase
{
public:
    enum CancellableTaskState
    {
        STAT_INIT,
        STAT_RUNNING,
        STAT_CANCELLED,
        STAT_FINISHED,
    };

    CancellableTask();
    virtual ~CancellableTask();
    
    virtual void Run();
    // CancelAsync
    void Cancel();
    // Cancel and wait until the task is really been cancelled
    void CancelWait();
    
    CancellableTaskState GetState();
private:
    // implement the DoRun interface and add something you want to execute here
    // you can call CheckCancellation in your DoRun interface to support cancel in other points
    virtual void DoRun() = 0;

    virtual void OnTaskFinished() {}
    virtual void OnTaskCancelled() {}

    void CheckCancellation();
    void SetState(CancellableTaskState state);
private:
    CancellableTaskState m_state;
    bool m_needCancel;

    MutexLock m_statMutex;
    Condition m_cancelCond;
    MutexLock m_cancelMutex;
};

#endif  // THREADPOOL_TASK_H_

