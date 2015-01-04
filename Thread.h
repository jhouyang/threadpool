#include <semaphore.h>
#include <pthread.h>

class ThreadBase
{
public:
    ThreadBase(bool bDetached = false);
    virtual ~ThreadBase();

    void Create();
    void Start();

    bool IsDetached() const;

    static void* _threadFunc(void* data);

    void SetDestroy();
    bool IsDestroyed() const;

    // real thread main loop should call this function to really stop the thread
    bool CheckDestroy() const;

    void WaitStart();
    void SignalStart();

    // this function is the entry point of thread
    virtual void Entry() = 0;

    pthread_mutex_t* GetMutex() const;
private:
    void DoCreate_unlocked();
    sem_t m_semStart; // used to control start
    sem_t m_semPause; // used to control pause/resume

    bool m_isDetached : 1; // thread is detached

    // states
    bool m_isStarted : 1; // if thread is started already
    bool m_isDestroyed : 1; // if thread is start to be destroyed
    bool m_isPaused : 1; // this is used for pause/start for the single thread

    pthread_mutex_t m_mutex;
    pthread_t m_threadID;
};
