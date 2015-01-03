#include <pthread.h>
#include <string>
#include "TasksQueue.h"

class TPool
{
public:
    explicit TPool(unsigned int tNumber) throw(std::string);

    void SetThreadNumber(unsigned int tNumber);

    // flow control functions
    // void Start();
    void Cancel();
    void Stop();
    void Resume();

    // tasks fun
    void AddTask(TaskBase* task);
private:
    void Init() throw(std::string);

private:
    TasksQueue m_taskQueue;
    unsigned int m_tNumber;
    bool m_bCancel : 1;
    pthread_t m_threadInfo;
};

