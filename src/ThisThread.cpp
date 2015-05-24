#include "ThisThread.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>

namespace
{
    int gettid()
    {
        return static_cast<int>(::syscall(SYS_gettid));
    }
} // anonymous namespace

namespace ThisThread
{
    // TODO : head file of __thread
    __thread int t_cachedId = 0;
    
    void cacheId()
    {
        if (t_cachedId == 0)
        {
            t_cachedId = ::gettid();
        }
    }

    bool isMainThread()
    {
        return tid() == ::getpid();
    }
} // namespace ThisThread
