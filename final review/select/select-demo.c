/**
 * After fork:
 * 
 * Child sends a number to parent once in a while, random interval.
 * 
 * Parent waits for input from both stdin (assume terminal and human user) and child,
 * prints what it receives each time.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>


int main() {

    pid_t p;
    int pipefd[2];

    pipe(pipefd);

    p = fork();

    // parent
    if (p != 0) {
        close(pipefd[1]);

        int i;

        FILE *child;
        child = fdopen(pipefd[0], "r");

        fd_set readfds;


        for (;;) {
            FD_ZERO(&readfds);              // clear the set
            FD_SET(0, &readfds);            // put stdin into readfds
            FD_SET(pipefd[0], &readfds);    // put pipe read-end into readfds

            if (select(pipefd[0] + 1, &readfds, NULL, NULL, NULL) < 0)
                continue;
                
            // if 0 is still in the readfds set after select return, 
            // which means we got something to read from 0 (stdin)
            if (FD_ISSET(0, &readfds)) {
                scanf("%d", &i);
                printf("user enters %d\n", i);
            }

            // if pipefd[0] is still in the set, we got something to read
            if (FD_ISSET(pipefd[0], &readfds)) {
                fscanf(child, "%d", &i);
                printf("child sends %d\n", i);
            }
            
        }
    } 
    

    // child
    else {
        close(pipefd[0]);
        int r;

        FILE *parent;

        parent = fdopen(pipefd[1], "w");
        for (;;) {
            
            sleep(2);
            fprintf(parent, "%d\n", 1);
            fflush(parent);
        }
    }


}