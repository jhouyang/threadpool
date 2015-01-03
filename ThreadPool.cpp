#include "ThreadPool.h"

namespace
{
    // functor to excute task
    class TaskExecutor
    {
    public:
        explicit TaskExecutor(TasksQueue* taskQueue)
            : m_taskQueue(taskQueue)
        {
        }

        void* operator() (void* data)
        {
            while (true)
            {
                TaskBase* task = m_taskQueue->PopTask();
                task->Do();
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
}

void TPool::Init() throw(std::string)
{
   // pthread_create
}
