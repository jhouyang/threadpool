#include <list>

class TaskBase
{
public:
    virtual ~TaskBase() { }
    virtual void Do() = 0;
    virtual bool IsTaskFinished() const = 0;
};

class Queue
{
public:
    Queue();
    ~Queue();

    // could be used by threads
    TaskBase* PopTasks();
    bool IsEmpty() const;

    // only used in main thread
    void PushTask(TaskBase* task);

private:
    std::list<TaskBase*> m_tasks;
};

