#include "ThreadPool.h"

namespace
{
    // template function for pthread_create
    template <typename ClassType>
    void* _threadFunc(void* param)
    {
        ClassType* ptr = (ClassType*)param;
        ptr->Execute();
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
    pthread_t threadInfo;
    unsigned int curNumber = 0;
    while (curNumber++ < m_tNumber)
    {
        ::TaskExecutor exec(&m_taskQueue);
        pthread_create(&threadInfo, NULL, ::_threadFunc< ::TaskExecutor >, &exec);
    }
    pthread_join(threadInfo, NULL);
}

