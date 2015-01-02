#include <pthread.h>

class TPool
{
public:
    explicit TPool(unsigned int tNumber);

    void SetThreadNumber(unsigned int tNumber);

    // flow control functions
    void Start();
    void Cancel();
    void Stop();
    void Resume();

private:
    void Init();

private:
    TasksQueue m_taskQueue;
    unsigned int m_tNumber;
};

