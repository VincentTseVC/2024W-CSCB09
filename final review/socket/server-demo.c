#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <signal.h>


/**
 * 4 Steps to set up a Server
 *  1. socket()
 *  2. bind()
 *  3. listen()
 *  4. accept() 
 **/

// #define PORT_NUM 5000
#define MAX_BACKLOG 5

void serve_client(int fd);

    
void ignore_sigpipe() {
    struct sigaction myaction;
    myaction.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &myaction, NULL);
}

int main(int argc, char **argv) {

    unsigned short port = (unsigned short) atoi(argv[1]);


    // Bad clients can close before we write. Ignore SIGPIPE to keep running.
    ignore_sigpipe();

    /** Create Socket. 
     * int socket(int family, int type, int protocol);
     * 
     * - family:
     *      AF_INET – IPv4
     *      AF_LOCAL - Unix domain
     * 
     * - type:  
     *      SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
     * 
     * - protocol:
     *      set to 0 except for RAW sockets
     * 
     * Return a socket descriptor
     **/
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    /** Bind socket to an address. 
     *  int bind(int sockfd, const struct sockaddr_in *servaddr, socklen_t addrlen);
     * 
     *  - sockfd:
     *      returned by socket
     * 
     *  - servaddr:
     *      an address of a following struct
     * 
     *          struct sockaddr_in {
     *              short           sin_family;     // AF_INET 
     *              u_short         sin_port;
     *              struct in_addr  sin_addr;
     *              char            sin_zero[8];    // unused, for padding..
     *          };
     * 
     *      sin_addr.s_addr can be set to INADDR_ANY to communicate on any network interface.
     * 
     *  - addrlen:
     *      is the size of struct sockaddr_in
     **/
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);  // Note use of htons here
    server.sin_addr.s_addr = INADDR_ANY;
    memset(&server.sin_zero, 0, sizeof(server.sin_zero));

    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    /** Create queue in kernel for new connection requests. 
     * int listen(int sockfd, int backlog);
     * 
     * - sockfd:
     *      returned by socket
     * 
     * - backlog:
     *      the maximum number of partially completed connections that the kernel should queue.
     **/ 
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }
    // sock_fd is now ready to accept connections!!!!!


    for (;;) {

        struct sockaddr_in client;  // address of client will be stored here.
        socklen_t sinlen;
        
        sinlen = sizeof(struct sockaddr_in);

        /** Accept a new connection. 
         * int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
         * 
         * - sockfd:
         *      returned by socket, the socket that is listeing to...
         * 
         * - cliaddr:
         *      stores the address of the client on return
         **/ 
        int client_fd = accept(sock_fd, (struct sockaddr *)&client, &sinlen);
        if (client_fd < 0) {
            perror("server: accept");
            close(sock_fd);
            exit(1);
        }

        printf("Server: got a new connection.\n");

        // Do anything you want with the server :)
        serve_client(client_fd);

        close(client_fd);

    }

    // close(sock_fd);

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
    write(fd, output, strlen(output));                  // (**)

}

// Note: This server is still naive. At (*), if client decides to chill out
// and not write, server stuck waiting, unresponsive to other clients.
//
// True of (**) too if server writes large chunk, larger than kernel buffer,
// and network or client too slow. Recall "write" blocks when kernel buffer full.
//
// Solution is the topic of I/O multiplexing.