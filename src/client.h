#include <sys/socket.h>

#ifndef H_CLIENT
#define H_CLIENT

struct client {
    int fd;
    struct sockaddr address;
};

struct client* client_alloc();
void client_free(struct client* client);

struct client* client_accept(int server_fd);
int client_disconnect(struct client* client);

#endif
