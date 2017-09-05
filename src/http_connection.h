#include <sys/socket.h>

#include "linked_list.h"
#include "http_request.h"

#ifndef H_HTTP_CONNECTION
#define H_HTTP_CONNECTION

struct http_connection {
    int conn_state;

    struct client* client;
    struct http_request req;

    char* body;
};

struct http_connection* http_connection_alloc(struct client* client);
struct linked_list* http_connection_accept(int server_fd, struct linked_list* connections);

void http_connection_release(struct linked_list* slot);
void http_connection_close(struct http_connection* conn);
void http_connection_free(struct http_connection* conn);

#endif
