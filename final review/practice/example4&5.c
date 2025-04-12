#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

// Example 5
void do_left(int fd_read , int fd_write) {
    char buff[512];

    int num_bytes;

    int max_fd = fd_read;
    fd_set all_fds, listen_fds;
    FD_ZERO(&all_fds);
    FD_SET(0, &all_fds);
    FD_SET(fd_read, &all_fds);

    while (1) {
        listen_fds = all_fds;

        select(max_fd + 1, &listen_fds, NULL, NULL, NULL);

        if (FD_ISSET(0, &listen_fds)) {
            if ((num_bytes = read(0, buff, 512)) <= 0) // EOF
                exit(0);
            else {
                for (int i =0; i < num_bytes; i++)
                    buff[i] = toupper(buff[i]);

                write(fd_write, buff, num_bytes);
            }
        }

        if (FD_ISSET(fd_read, &listen_fds)) {
            if ((num_bytes = read(fd_read, buff, 512)) <= 0) // EOF
                exit(0);
            else {
                write(1, buff, num_bytes); // printf(...)
            }
        }
    }
}


// Example 4
void leftright(void) {
    // parent begins
    int ltr [2];
    int rtl [2];

    pipe(ltr);
    pipe(rtl);

    pid_t left = fork ();
    if (left == 0) {
        // left child code , part (a)
        close(rtl[1]);
        close(ltr[0]);

        do_left(rtl[0], ltr[1]);

        close(ltr[1]);
        close(rtl[0]);

    } else {
        pid_t right = fork ();
        if (right == 0) {
            // right child code , part (b)
            close(ltr[1]);
            close(rtl[0]);

            dup2(ltr[0], 0);
            dup2(rtl[1], 1);

            close(ltr[0]);
            close(rtl[1]);

            execlp (" right" , " right" , (char *) NULL);
            exit (1);
        } else {
            // parent code after both forks , part (c)
            close(ltr[0]);
            close(ltr[1]);

            close(rtl[0]);
            close(rtl[1]);

            wait(NULL);
            wait(NULL);
        }
    }
}
