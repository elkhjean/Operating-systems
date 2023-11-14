#include <mqueue.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MSG_SIZE 1000 // Max size of a messages to be enqueued
#define MAX_NUM_MSG 10    // Max number of messages allowed on the queue
#define Q_NAME "/mymq"    // Name of message queue
#define MQ_MODE 0644      // Read and write permission for owner, read for rest

int countWords(char *str)
{
    int wordCount = 0;
    int inWord = 0; // 0 means currently in a whitespace, 1 means in a word
    while (*str)
    {
        if (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\r') // Detects whitespace
        {
            inWord = 0;
        }
        else if (inWord == 0) // If we are not in a word ransition from whitespace to word and count 1
        {
            inWord = 1;
            wordCount++;
        }
        str++;
    }
    return wordCount;
}

int main(int argc, char **argv)
{
    char *file_name = argv[1];
    mqd_t mqd;
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_NUM_MSG;
    attr.mq_msgsize = MAX_MSG_SIZE;
    char buf[MAX_MSG_SIZE];
  
//    mqd = mq_open(Q_NAME, O_RDWR | O_CREAT, MQ_MODE, &attr);

    pid_t pid;
    pid = fork();

    if (pid == -1)
    { /* Error handling */
        perror("fork error");
        exit(1);
    }
    else if (pid == 0)
    { /* child process */

        mqd = mq_open(Q_NAME, O_WRONLY | O_CREAT, MQ_MODE, &attr);

        FILE *file = fopen(file_name, "r"); // Opens file in reading mode
        if (file == NULL)
        {
            perror("Error opening file");
            exit(1);
        }
        fread(buf, 1, MAX_MSG_SIZE, file); // Read up to MAX_MSG_SIZE bytes from file into buf 1 byte at a time
        fclose(file);

        int send = mq_send(mqd, buf, MAX_MSG_SIZE, 0); // Send up to MAX_MSG_SIZE number of bytes from buffer with prio 0 to the queue mqd
        if (send < 0)
        {
            perror("Error sending message");
            exit(1);
        }
        mq_close(mqd);
        return 0;
    }

    else if (pid > 0)
    { /* parent process */
        mqd = mq_open(Q_NAME, O_RDONLY | O_CREAT, MQ_MODE, &attr);

        int rcv = mq_receive(mqd, buf, MAX_MSG_SIZE, NULL);
        if (rcv < 0)
        {
            perror("Error receiving message");
            exit(1);
        }
        printf("%d\n", countWords(buf));
        mq_close(mqd);
        mq_unlink(Q_NAME);
        return 0;
    }
}