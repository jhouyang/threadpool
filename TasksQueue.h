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

    void PushTask(TaskBase* task);
    TaskBase* PopTasks();
    bool IsEmpty() const;

private:
    std::list<TaskBase*> m_tasks;
};

