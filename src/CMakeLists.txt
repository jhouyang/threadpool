INCLUDE_DIRECTORIES(
    /Users/jouyang/Downloads/boost_1_57_0/ 
    /Users/jouyang/gtpool_plus/include/)

SET(SRC_LIST Locks.cpp
        TasksQueue.cpp
        Task.cpp
        Thread.cpp
        ThreadPool.cpp
        ThisThread.cpp
        TaskBlockingQueue.cpp
        )

ADD_LIBRARY(threadpool SHARED ${SRC_LIST})
