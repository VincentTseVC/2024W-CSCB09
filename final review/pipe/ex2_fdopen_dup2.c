#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Basically "echo "...." | sort" */

int main() {

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe :(");
        return 1;
    }

    pid_t p;
    if ((p = fork()) == -1) {
        perror("fork :(");
        return 1;
    } else if (p != 0) {
        // parent doesn't need read end..
        close(pipefd[0]);

        // write some message to the write-end
        FILE *out;
        out = fdopen(pipefd[1], "a");
        fprintf(out, "hello there\n");
        fprintf(out, "aloha everyone\n");
        fprintf(out, "ni hao\n");
        fprintf(out, "bonjour\n");
        fclose(out);

        wait(NULL); // wait for child to be sure :)

        return 0;
    } else {
        // Child doesn't need write-end
        close(pipefd[1]);

        // Redirect stdin from read-end 
        dup2(pipefd[0], 0);

        // Doesn't need original read-end FD, 0 (stdin) has read end now :)
        close(pipefd[0]);

        if (execlp("sort", "sort", (char *)NULL) == -1) {
            perror("exec :(");
            return 1;
        }
        return 0;
    }
}