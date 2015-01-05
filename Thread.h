#include <semaphore.h>
#include <pthread.h>

enum ThreadState
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
    
    // state query
    void SetState(ThreadState state);
    ThreadState GetState() const;
private:
    void DoCreate_unlocked();

private:
    ThreadState m_state;
    sem_t m_semStart; // used to control start
    sem_t m_semPause; // used to control pause/resume

    bool m_isDetached : 1; // thread is detached

    // want states
    bool m_isDestroyed : 1; // if thread is start to be destroyed
    bool m_isPaused : 1; // this is used for pause/start for the single thread

    // this lock is for m_state and thread start/create 
    // FIXME : maybe it's no need to add mutex to protect start/create ? Not sure, need verify.
    pthread_mutex_t m_mutex;
    
    // the new created threadID
    pthread_t m_threadID;
};
