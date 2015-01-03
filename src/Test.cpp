#include "ThreadPool.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

class PrintTask : public TaskBase
{
    virtual void Do()
    {
        printf("I'm PrintTask %p\n", this);
    }
};

// template function for pthread_create
template <typename ClassType>
void* _threadFunc(void* param)
{
    ClassType* ptr = (ClassType*)param;
    ptr->Execute();
    return NULL;
}

// functor to excute task
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

int main()
{
    /*
    TasksQueue tq;
    TaskExecutor exec(&tq);
    pthread_t tid;
    pthread_create(&tid, NULL, _threadFunc<TaskExecutor>, &exec);
    pthread_create(&tid, NULL, _threadFunc<TaskExecutor>, &exec);
    pthread_create(&tid, NULL, _threadFunc<TaskExecutor>, &exec);
    pthread_create(&tid, NULL, _threadFunc<TaskExecutor>, &exec);

    sleep(1);
    pthread_t tid2 = pthread_self();
    printf("main tid %p\n", tid2);
    tq.PushTask(new PrintTask());
    sleep(2000);
    */
    TPool tp(4);
    sleep(1);

    tp.AddTask(new PrintTask());
    tp.AddTask(new PrintTask());
    tp.AddTask(new PrintTask());
    tp.AddTask(new PrintTask());
    sleep(2000);
    return 0;
}

