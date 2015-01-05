#include <semaphore.h>
#include <pthread.h>

enum ThreadStats
{
    STAT_NEW,     // haven't call pthread_create
    STAT_CREATED, // already call pthread_create but wait for the Start() to wakeup
    STAT_RUNNING, // IsRuning 
    STAT_PAUSE,   // Is Paused
    STAT_EXIT,    // under exit state
};

// TODO: although we have these five kinds of state, but cause we use signal to pause / resume/ stop thread.
// And the Entry() function can be under infinite loop, it need Call CheckDestropy to make the state really works.
// Therefore, we still need other flags for these kinds of usage.
// The flag should, m_isStoped / m_isPaused.
// I need to reorder these pieces of code.

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
