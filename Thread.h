#include <semaphore.h>
#include <pthread.h>

class ThreadBase
{
public:
    ThreadBase(bool bDetached = false);
    ~ThreadBase();

    Creat();
    Start();

    bool IsDetached() const;

    static void* _threadFunc(void* data);

    void SetDestroy();
    bool IsDestroyed() const;

    // real thread main loop should call this function to really stop the thread
    bool CheckDestroy() const;

    void StartWait();
    void SignalStart();

    // this function is the entry point of thread
    virtual void DoRun() = 0;

private:
    sem_t m_semStart; // used to control start
    sem_t m_semPause; // used to control pause/resume

    bool m_isDetached : 1; // thread is detached
    bool m_isStarted : 1; // if thread is started already

    bool m_isDestroyed : 1; // if thread is start to be destroyed

    bool m_isPaused : 1;

    pthread_mutex_t m_mutex;
    pthread_t m_threadID;
};
