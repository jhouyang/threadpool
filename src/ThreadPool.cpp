#include "ThreadPool.h"
#include "Thread.h"
#include <assert.h>

namespace
{
    // template function for pthread_create
    template <typename ClassType>
    void* _threadFunc(void* param)
    {
        ClassType* ptr = (ClassType*)param;
        ptr->Execute();
        delete ptr;
        return NULL;
    }

    // functor to excute task
    // TODO : we may need other kinds of TaskExecutor
    class TaskExecutor
    {
    public:
        explicit TaskExecutor(TasksQueue* taskQueue)
            : m_taskQueue(taskQueue)
        {
        }

        void Execute()
        {
            while (true)
            {
                // thread may wait here for available task
                TaskBase* task = m_taskQueue->PopTask();
                assert(task);

                task->Run();
                delete task;
            }
        }
    private:
        TasksQueue* m_taskQueue;
    };
}

TPool::TPool(unsigned int tNumber) throw(std::string)
    : m_tNumber(tNumber)
{
    Init();
}

TPool::~TPool()
{
    Stop(false);
}

void TPool::Init() throw(std::string)
{
    unsigned int curNumber = 0;
    while (curNumber++ < m_tNumber)
    {
        ThreadBase* th = new DefaultThread(this, true);
        m_workers.push_back(th);
        th->Start();
    }
}

void TPool::Pause()
{
    m_taskQueue.Pause();
}

void TPool::Resume()
{
    m_taskQueue.Resume();
}

void TPool::Stop(bool bForce)
{
    std::list<ThreadBase*>::iterator it;
    for (it = m_workers.begin(); it != m_workers.end(); )
    {
        ThreadBase* ptr = *it;
        ptr->Cancel();
        it = m_workers.erase(it);
        delete ptr;
    }
}

void TPool::SetThreadNumber(unsigned int tNumber)
{
    // TODO: how to rebuild threadpool?
}

void TPool::AddTask(TaskBase* task)
{
    m_taskQueue.PushTask(task);
}

TaskBase* TPool::GetTask()
{
    return m_taskQueue.PopTask();
}

