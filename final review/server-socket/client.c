#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MY_PORT 6000

int main(int argc, char **argv) {

    // if (argc < 2) {
    //     perror("client: Need server IPv4 address.");
    //     return 1;
    // }

    // char *adr = "192.168.0.107";
    char *adr = "127.0.0.1";

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    unsigned short port = (unsigned short)atoi(argv[1]);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, adr, &server.sin_addr) == 0) {
        perror("client: That's not an IPv4 address.");
        close(sock_fd);
        exit(1);
    }

    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: cannot connect.");
        close(sock_fd);
        return 1;
    }

    // char *input = "ls -l";

    write(sock_fd, argv[2], strlen(argv[2]));


    char output[1024];
    int n = read(sock_fd, output, 1024);
    output[n] = '\0';

    printf("%s\n", output);
    close(sock_fd);
    return 0;
}
