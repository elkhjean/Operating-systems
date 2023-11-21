// use flags -lrt and -lpthread when compiling

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX 3
#define SIZE (sizeof(struct shared_memory))

const char *SHM_NAME = "myshm";
const char *MUTEX_NAME = "/mutex";
const char *RW_MUTEX_NAME = "/rw_mutex";

sem_t *mutex, *rw_mutex;

struct shared_memory
{
    int VAR;
    int read_count;
};

/*
    Reader_process function, first, locks the semaphore for the reader counter and updates it.
    If it is the first reader to initialize a read, it locks the the rw semaphore for writing
    and reads the value. If it is not the first reader, the variable is simply read, no new
    lock is acquired since one already exits. The lock is maintained until the last reader
    has read the variable. The lock is then released and writing is once again enabled.
    */
void reader_process(struct shared_memory *shared)
{
    do
    {
        usleep(300);
        sem_wait(mutex);
        shared->read_count++;
        if (shared->read_count == 1)
        {
            sem_wait(rw_mutex);
            printf("The first reader acquires the lock\n");
        }
        sem_post(mutex);

        printf("The reader (%d) reads the value %d\n", getpid(), shared->VAR);

        sem_wait(mutex);
        shared->read_count--;
        if (shared->read_count == 0)
        {
            sem_post(rw_mutex);
            printf("The last reader releases the lock\n");
        }
        sem_post(mutex);
    } while (shared->VAR < MAX);
}
/* Writer_process function, locks read/write semaphore and writes to the shared variable VAR*/
void writer_process(struct shared_memory *shared)
{
    do
    {
        usleep(250);
        sem_wait(rw_mutex);
        printf("The writer acquires the lock\n");
        shared->VAR++;
        printf("The writer (%d) writes the value %d\n", getpid(), shared->VAR);
        sem_post(rw_mutex);
        printf("The writer releases the lock\n");
    } while (shared->VAR < MAX);
}

int main(void)
{
    // Initialization
    mutex = sem_open(MUTEX_NAME, O_CREAT, 0644, 1);
    rw_mutex = sem_open(RW_MUTEX_NAME, O_CREAT, 0644, 1);
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    struct shared_memory *shared = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    shared->read_count = 0;
    shared->VAR = 0;

    // Forking
    pid_t pid1 = fork();
    if (pid1 == 0)
    { // Child 1
        reader_process(shared);
        return 0;
    }
    pid_t pid2 = fork();
    if (pid2 == 0)
    { // Child 2
        reader_process(shared);
        return 0;
    }

    // Parent
    writer_process(shared);
    wait(NULL);
    wait(NULL);

    // Cleanup
    munmap(shared, SIZE);
    close(fd);
    shm_unlink(SHM_NAME);
    sem_close(mutex);
    sem_close(rw_mutex);
    sem_unlink(MUTEX_NAME);
    sem_unlink(RW_MUTEX_NAME);

    return 0;
}
