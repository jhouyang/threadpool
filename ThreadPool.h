#ifndef THREADPOOL_TPOOL_H_
#define THREADPOOL_TPOOL_H_

#include <pthread.h>
#include <string>
#include <list>
#include "TasksQueue.h"

class ThreadBase;

class TPool
{
public:
    explicit TPool(unsigned int tNumber) throw(std::string);
    ~TPool();

    void SetThreadNumber(unsigned int tNumber);

    // flow control functions
    // set bForce = false will just stop accept new tasks but will execute all tasks in TasksQueue
    // set bForce = true; won't accept any new tasks
    void Stop(bool bForce = true);
    void Pause();
    void Resume();

    // tasks fun
    void AddTask(TaskBase* task);
    TaskBase* GetTask();
private:
    void Init() throw(std::string);

private:
    TasksQueue m_taskQueue;
    unsigned int m_tNumber;
    bool m_bCancel : 1;

    std::list<ThreadBase*> m_workers;
};
#endif  // THREADPOOL_TPOOL_H_


