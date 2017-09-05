#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "utils.h"

#define MAX_BACKLOG 20

int server_listen(const char* portnumber, const char* address) {
    struct addrinfo hints;
    struct addrinfo* addr;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    if (getaddrinfo(address, portnumber, &hints, &addr) != 0)
        return fail_with_error("Failed to retrieve addrinfo: %s\n", 1);

    int sock_fd = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (sock_fd < 0)
        return fail_with_error("Failed to create socket: %s\n", 2);

    int reuse = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(sock_fd, addr->ai_addr, addr->ai_addrlen) != 0)
        return fail_with_error("Failed to bind: %s\n", 3);

    freeaddrinfo(addr);

    if (listen(sock_fd, MAX_BACKLOG) != 0)
        return fail_with_error("Failed to listen: %s\n", 3);

    if (address != NULL)
        printf("Listening to %s:%s\n", address, portnumber);
    else
        printf("Listening to 0.0.0.0:%s\n", portnumber);

    return sock_fd;
}
