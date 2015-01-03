#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool bGlobal = false;
void* thr_fun(void* data)
{
    pthread_mutex_lock(&mutex);
    if (!bGlobal)
    {
        pthread_t tid = pthread_self();
        printf("tid %p\n", tid);
        pthread_cond_wait(&cond, &mutex);
        printf("return from cond_wait\n");
    }
    printf("release lock\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    pthread_t threadInfo;
    pthread_create(&threadInfo, NULL, thr_fun, NULL);

    sleep(10);
    pthread_mutex_lock(&mutex);
    printf("I get Mutex\n");
    pthread_mutex_unlock(&mutex);

    return 0;
}
