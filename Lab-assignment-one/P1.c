#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

    int fd[2];
    pipe(fd);

    pid_t pid;
    pid = fork();

    if (pid == -1)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid == 0)
    { /* child process */

        close(fd[0]);
        dup2(fd[1], 1);
        execlp("ls", "ls", "/", NULL);
        return 0;
    }
    else if (pid > 0)
    { /* parent process */

        close(fd[1]);
        dup2(fd[0], 0);
        wait(NULL);
        execlp("wc", "wc", "-l", NULL);
        return 0;
    }
}