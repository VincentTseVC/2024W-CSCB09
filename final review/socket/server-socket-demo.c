#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <signal.h>
#include <sys/select.h>

#define PORT_NUM 5000
#define MAX_BACKLOG 5

void serve_client(int fd);


int main() {

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);  // Note use of htons here
    server.sin_addr.s_addr = INADDR_ANY;
    memset(&server.sin_zero, 0, sizeof(server.sin_zero));

    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    fd_set readfds, activefds;
    FD_ZERO(&activefds);
    FD_SET(sock_fd, &activefds);
    for (;;) {
        // make a copy of activefds set
        readfds = activefds;

        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) continue;

        for (int fd = 3; fd < FD_SETSIZE; fd++) {

            if (FD_ISSET(fd, &readfds)) {
                if (fd == sock_fd) {
                    /* Connection request on original socket... */
                    struct sockaddr_in client;
                    socklen_t sinlen;
                    sinlen = sizeof(struct sockaddr_in);

                    int client_fd = accept(sock_fd, (struct sockaddr *)&client, &sinlen);
                    if (client_fd < 0) {
                        perror("server: accept");
                        close(sock_fd);
                        exit(1);
                    }
                    // add the new client_fd to the set
                    FD_SET(client_fd, &activefds);
                    printf("Server: got a new connection.\n");

                }
                else {
                    /* Data arriving on an already-connected socket (client_fd) */
                    serve_client(fd);
                    FD_CLR(fd, &activefds);
                    close(fd);
                }
            }
        }
    }
    return 0;
}

void serve_client(int fd) {

    char *prefix = "Here you go, ";
    char *suffix = ". Have a good day!\r\n";    // remember :)
    char buf[128];

    // Bad clients can close before we read, always check return value.
    int num_read;
    if ((num_read = read(fd, buf, 127)) <= 0) return;   // (*)

    buf[num_read] = '\0';

    printf("Server received the following order: %s\n", buf);

    // If bad client closes before we write, we ignore SIGPIPE, then write

    // write: "Here you go, [order]. have a good day!"

    char output[128];
    strcpy(output, prefix);
    strcat(output, buf);
    strcat(output, suffix);
    write(fd, output, strlen(output));

}

