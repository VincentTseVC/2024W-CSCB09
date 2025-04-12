#include <stdio.h>
#include <unistd.h>

int main() {

    int fd[2];

    pipe(fd);

    pid_t p;

    p = fork();

    // parent
    if (p > 0) {
        close(fd[0]);   // close the read-end
        write(fd[1], "hello", 6);
        close(fd[1]);
    }

    // child
    else if (p == 0) {
        close(fd[1]);   // close the write-end
        
        char res[10];
        read(fd[0], res, 10);

        printf("read: %s\n", res);
        close(fd[0]);

    }
}