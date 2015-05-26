#ifndef THISTHREAD_H_
#define THISTHREAD_H_

#ifndef unlikely
#define unlikely(x) \
    __builtin_expect(!!(x), 0)
#endif

namespace ThisThread
{
    extern __thread int t_cachedId;
    void cacheTid();

    inline int tid()
    {
        if (unlikely(t_cachedId == 0))
        {
            cacheTid();
        }
        return t_cachedId;
    }

    bool isMainThread();
}
#endif  // THISTHREAD_H_

