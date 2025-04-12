#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// #define MY_PORT 5000

int main(int argc, char **argv) {

    if (argc < 3) {
        perror("client: Need server IPv4 address.");
        return 1;
    }

    unsigned short port = (unsigned short) atoi(argv[2]);


    /** Create Socket.
     * (check server-demo.c for details)
     **/
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    /** Create struct representing the server.
     * (check server-demo.c for details)
     **/
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);


    /** Convert the character string into a network address structure (IPv4 or IPv6).
     *
     * int inet_pton(int af, const char *src, void *dst);
     *
     * This function converts the character string src into a network
     * address structure in the af address family, then copies the
     * network address structure to dst.  The af argument must be either
     * AF_INET or AF_INET6.  dst is written in network byte order.
     *
     * return
     *  1   on success (network address was successfully converted).
     *  0   if src does not contain a character string representing a valid network address in the specified address family.
     *  -1  If af does not contain a valid address family, and errno is set to EAFNOSUPPORT.
     **/
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) == 0) {
        perror("client: That's not an IPv4 address.");
        close(sock_fd);
        exit(1);
    }

    /** request connection to server.
     *
     * int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
     *
     * returns 0 on success and -1 on failure setting errno.
     **/
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: cannot connect.");
        close(sock_fd);
        return 1;
    }

    printf("Client: I'm connected now\n");

    char *order = (argc > 3) ? argv[3] : "cheese burger";

    write(sock_fd, order, strlen(order));

    char buf[128];
    int num_read = read(sock_fd, buf, 127);
    buf[num_read] = '\0';

    printf("Cient: server said... %s\n", buf);
    close(sock_fd);
    return 0;
}
