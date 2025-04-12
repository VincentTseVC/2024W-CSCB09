#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */

#define MAXCLIENTS 10
#define MAXLINE 1024
#define MAXOUTPUT 1048576  /* 1MB max size */

struct clientstate {
    int sock;     /* Socket to write to */
    int fd[2];    /* The pipe descriptors for the child to write to parent */
    char *path;   /* program to run */
    char *output; /* pointer to the beginning of the response data */
    char *optr;   /* pointer to the current end of the response data */
};

void initClients(struct clientstate *client, int size) {
    // Initialize client array
    for (int i = 0; i < size; i++){
        client[i].sock = -1;	/* -1 indicates available entry */
        client[i].fd[0] = -1;
        client[i].path = NULL;
        client[i].output = NULL;
        client[i].optr = client[i].output;
    }
}

/* Reset the client state cs.
 * Free the dynamically allocated fields
 */
void resetClient(struct clientstate *cs){
    cs->sock = -1;
    cs->fd[0] = -1;
    if(cs->path != NULL) {
        free(cs->path);
        cs->path = NULL;
    }
    if(cs->output != NULL) {
        free(cs->output);
        cs->output = NULL;
    }
}


// Helper function that accepts connections.
int accept_connection(int fd, struct clientstate *clients) {
    int i = 0;
    while (i < MAXCLIENTS && clients[i].sock != -1) {
        i++;
    }

    if (i == MAXCLIENTS) {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    clients[i].sock = client_fd;
    return client_fd;
}


/*
 * Create and set up a socket for a server to listen on.
 */
int setupServerSocket(unsigned short port) {
    int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }
    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr;

    // Allow sockets across machines.
    addr.sin_family = PF_INET;
    // The port the process will listen on.
    addr.sin_port = htons(port);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr.sin_zero), 0, 8);
    // Listen on all network interfaces.
    addr.sin_addr.s_addr = INADDR_ANY;

    // Associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        // bind failed; could be because port is in use.
        perror("bind");
        exit(1);
    }

    // Set up a queue in the kernel to hold pending connections.
    if (listen(soc, MAXCLIENTS) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }
    return soc;
}


int processRequest(struct clientstate *cs) {

    cs->output = malloc(MAXOUTPUT);
    cs->output[0] = '\0';
    cs->optr = cs->output;

    char line[MAXLINE + 1];

    int num_read = read(cs->sock, line, MAXLINE);
    line[num_read] = '\0';

    // // remove \r\n ending
    // if (line[num_read-1] == '\n') {
    //     line[num_read-1] = '\0';
    //     if (line[num_read-2] == '\r')
    //         line[num_read-2] = '\0';
    // }

    char *argv[20];

    int a = 0;
    char *token = strtok(line, " ");
    while (token != NULL) {
        argv[a++] = token;
        token = strtok(NULL, " ");
    }
    argv[a] = 0;

    // Check if the program requested is in the allowed set
    if (pipe(cs->fd) == -1) {
        perror("pipe");
        return -1;
    }

    int result = fork();
    if (result == -1) {
        fprintf(stderr, "Fork failed\n");
        return -1;
    } else if (result == 0) {
        dup2(cs->fd[1], fileno(stdout));
        close(cs->fd[0]);
        close(cs->fd[1]);
        execvp(argv[0], argv);

    } else { // parent
        close(cs->fd[1]);
        return cs->fd[0];
    }
    return -1; /* should never get here */
}

void printOutput(int fd, char *output, int length) {
    int n;
    while(length > MAXLINE) {
        n = write(fd, output, MAXLINE);
        length -= n;
        output += n;
    }
    n = write(fd, output, length);
    if(n != length) {
        perror("write");
    }
}


void printError(int fd) {

    char *error_str = "something went wrong ....";

    int result = write(fd, error_str, strlen(error_str));
    if(result != strlen(error_str)) {
        perror("write");
    }
}

int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: wserver <port>\n");
        exit(1);
    }
    unsigned short port = (unsigned short)atoi(argv[1]);
    int sfd;


    struct clientstate clients[MAXCLIENTS];


    // Set up the socket to which the clients will connect
    sfd = setupServerSocket(port);

    initClients(clients, MAXCLIENTS);

    int max_fd = sfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sfd, &all_fds);

    while (1) {
        fd_set listen_fds = all_fds;
        if (select(max_fd + 1, &listen_fds, NULL, NULL, NULL) == -1) {
            perror("server: select");
            exit(1);
        }

        // Create a new connection ...
        if (FD_ISSET(sfd, &listen_fds)) {
            // open client_fd
            int client_fd = accept_connection(sfd, clients);

            // update max_fd, if bigger
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }

            // add client_fd to set
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection [%d]\n", client_fd);
        }

        // Client socket
        for (int i = 0; i < MAXCLIENTS; i++) {
            if (clients[i].sock > -1 && FD_ISSET(clients[i].sock, &listen_fds)) {

                int pfd = processRequest(&clients[i]); //  read-end
                // if (pfd > max_fd)
                //     max_fd = pfd;

                resetClient(&clients[i]);
                max_fd = 0;
                for (int i = 0; i < MAXCLIENTS; i++) {
                    if (clients[i].sock > max_fd)
                        max_fd = clients[i].sock;
                }

                FD_SET(pfd, &all_fds);
                FD_CLR(clients[i].sock, &all_fds);
            }
        }

        // Pipe descriptor
        for (int i = 0; i < MAXCLIENTS; i++) {
            if (clients[i].sock > -1 && FD_ISSET(clients[i].fd[0], &listen_fds)) {
                // printf("reading from exec()\n");
                int num_read;
                if ((num_read = read(clients[i].fd[0], clients[i].optr, MAXLINE)) <= 0) {

                    // HERE IS A BUG
                    int status;
                    wait(&status);
                    if (WIFEXITED(status)) {
                        int ret = WEXITSTATUS(status);
                        if (ret == 0)
                            printOutput(clients[i].sock, clients[i].output, clients[i].optr - clients[i].output);
                        else
                            printError(clients[i].sock);

                    // WIFSIGNALED
                    } else {
                        printError(clients[i].sock);
                    }


                    close(clients[i].sock);
                    close(clients[i].fd[0]);
                    // FD_CLR(clients[i].sock, &all_fds);
                    FD_CLR(clients[i].fd[0], &all_fds);
                    resetClient(&clients[i]);
                }
                else {
                    clients[i].optr += num_read;
                }

            }
        }

    }
    return 0;
}



