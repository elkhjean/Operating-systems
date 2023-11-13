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
    { /* Error handling */

        perror("fork error");
        exit(1);
    }

    else if (pid == 0)
    { /* child process */

        close(fd[0]);                   // Close read end of the pipe
        dup2(fd[1], 1);                 // Redirect output from stdout (1) to write end of pipe (fd[1]) to pipe it to parent
        execlp("ls", "ls", "/", NULL);  // Replace child process memory with program ls with argument "/"
        return 0;
    }

    else if (pid > 0)
    { /* parent process */

        close(fd[1]);                   // Close write end of the pipe
        dup2(fd[0], 0);                 // Redirect read from stdin (0) to read end of pipe (fd[0]) to read from pipe
        wait(NULL);                     // Wait for child to complete
        execlp("wc", "wc", "-l", NULL); // Replace parent process memory with program wc with argument "-l"
        return 0;
    }
}