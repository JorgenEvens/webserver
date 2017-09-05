#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"

struct client* client_alloc() {
    return (struct client*)malloc(sizeof(struct client));
}

void client_free(struct client* client) {
    free(client);
}

struct client* client_accept(int server_fd) {
    struct client* client = client_alloc();

    struct sockaddr* remote_addr = &client->address;
    socklen_t addrlen = sizeof(struct sockaddr);

    memset(remote_addr, 0, addrlen);

    int fd = accept(server_fd, remote_addr, &addrlen);

    client->fd = fd;

    return client;
}

int client_disconnect(struct client* client) {
    close(client->fd);
    client_free(client);

    return 0;
}
