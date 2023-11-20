#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_MOD_SIZE 15

int buffer = 0;
pthread_mutex_t lock;

void *thread_func(void *arg)
{
    int ctr = 0;

    while (1)
    {
        pthread_mutex_lock(&lock);
        if (buffer < BUFFER_MOD_SIZE)
        {
            printf("TID: %ld, PID: %d, Buffer: %d\n", pthread_self(), getpid(), buffer);
            buffer++;
            ctr++;
        }
        else
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);
        usleep(3);
    }

    printf("TID %ld worked on the buffer %d times\n", pthread_self(), ctr);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2, t3;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&t1, NULL, &thread_func, NULL);
    pthread_create(&t2, NULL, &thread_func, NULL);
    pthread_create(&t3, NULL, &thread_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}
