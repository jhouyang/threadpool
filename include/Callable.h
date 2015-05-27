#ifndef CALLABLE_H_
#define CALLABLE_H_

template <typename T>
class Callable
{
public:
    virtual ~Callable() {}
    virtual T call() = 0;
};

class Runnable
{
public:
    virtual ~Runnable() {}
    virtual void run() = 0;
};

template <>
class Callable<void>
{
public:
    Callable(Runnable& run) 
        : m_run(run)
    {
    }
    
    void call() {
        m_run.run();
    }
private:
    Runnable& m_run;
};
#endif  // CALLABLE_H_
