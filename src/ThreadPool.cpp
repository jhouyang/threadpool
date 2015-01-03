#include "ThreadPool.h"
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
                pthread_t tid = pthread_self();
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
    Init();
}

TPool::~TPool()
{
    Stop();
}

void TPool::Init() throw(std::string)
{
    unsigned int curNumber = 0;
    while (curNumber++ < m_tNumber)
    {
        // will be deleted in _threadFunc
        ::TaskExecutor *exec = new ::TaskExecutor(&m_taskQueue);
        if (pthread_create(&m_threadInfo, NULL, ::_threadFunc< ::TaskExecutor >, exec))
        {
            std::string errorMsg = "Failed to create thread";
            throw errorMsg;
        }
    }
}

void TPool::Cancel()
{
    m_taskQueue.SetCancel(true);
}

void TPool::Resume()
{
    m_taskQueue.SetCancel(false);
}

void TPool::Stop()
{
    // FIXME: will it work?
    pthread_cancel(m_threadInfo);
}

void TPool::SetThreadNumber(unsigned int tNumber)
{
    // TODO: how to rebuild threadpool?
}

void TPool::AddTask(TaskBase* task)
{
    m_taskQueue.PushTask(task);
}


