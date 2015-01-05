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

// TODO : about mutex, currently I use one mutex to lock nearly all public interface.
//        Maybe it's no need to lock some places, and even I have to, I can consider to use more than one mutex to lock

class ThreadBase
{
public:
    ThreadBase(bool bDetached = false);
    virtual ~ThreadBase();

    void Create();
    void Start();

    bool IsDetached() const;

    static void* _threadFunc(void* data);
    
    // Destroy current thread (kill it)
    void Destroy();
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
    
    // set Pause flag but won't really pause unless CheckDestroy is called
    void Pause();
    // resume from pause
    void Resume();
private:
    void DoCreate_unlocked();
    void HungupThread(); // really pause

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
