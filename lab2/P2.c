// Link with -lrt -lpthrea
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX 5
int SIZE = sizeof(int) * 2;
char *shm = "myshm";
sem_t *mutex, *rw_mutex;

struct shared_memory
{
    int VAR;
    int read_count;
};

void *reader_process(void *arg)
{
    struct shared_memory *shared = (struct shared_memory *)arg;
    do
    {
        sem_wait(mutex);      // Protect read_count
        shared->read_count++; // Increase reade_count
        if (shared->read_count == 1)
        {                       // If this is the FIRST reader
            sem_wait(rw_mutex); // Waits to get access, Lock readWrite
            printf("The first reader acquires the lock\n");
        }
        sem_post(mutex); // Release read_coount

        printf("The reader (%d) reads the value %d\n", getpid(), shared->VAR); // read from to the shared object
        sem_wait(mutex);                                                       // Lock reader to protect read_count
        shared->read_count--;                                                  // Decrement read_count
        if (shared->read_count == 0)
        {                       // If this is the LAST reader
            sem_post(rw_mutex); //      Unlock readWrite
            printf("The last reader releases the lock\n");
        }
        sem_post(mutex); // Unlock reader, signal
        usleep(3);
    } while (shared->VAR < MAX);
}

void *writer_process(void *arg)
{
    struct shared_memory *shared = (struct shared_memory *)arg;
    do
    {
        sem_wait(rw_mutex); // Lock readWrite
        printf("The writer acquires the lock\n");
        shared->VAR++;
        printf("The writer (%d) writes the value %d\n", getpid(), shared->VAR);
        sem_post(rw_mutex); // Unlock reaadWrite
        printf("The writer releases the lock\n");
        usleep(1);
    } while (shared->VAR < MAX);
}

int main(void)
{

    mutex = sem_open("/mutex", O_CREAT, 0644, 1);
    rw_mutex = sem_open("/rw_mutex", O_CREAT, 0644, 1);

    int fd = shm_open(shm, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    struct shared_memory *shared = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
    shared->read_count = 0;
    shared->VAR = 0;

    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0)
    { // child1
        reader_process(shared);
    }
    else
    {
        pid2 = fork();
        if (pid2 == 0)
        { // child2
            reader_process(shared);
        }
        else
        { // parent
            writer_process(shared);
            wait(NULL);
            wait(NULL);
            munmap(shared, SIZE);
            close(fd);
            sem_close(mutex);
            sem_close(rw_mutex);
            sem_unlink("/mutex");
            sem_unlink("/rw_mutex");
        }
    }
    return 0;
}
