#include <string.h>
#include <stdlib.h>

#include "client.h"
#include "linked_list.h"
#include "http_connection.h"

struct http_connection* http_connection_alloc(struct client* client) {
    struct http_connection* conn = (struct http_connection*)malloc(sizeof(struct http_connection));
    memset(conn, 0, sizeof(struct http_connection));

    conn->client = client;
    return conn;
}

void http_connection_free(struct http_connection* conn) {
    http_request_release(&conn->req);

    if (conn->client != NULL)
        client_disconnect(conn->client);

    free(conn);
}

struct linked_list* http_connection_accept(int server_fd, struct linked_list* connections) {
    struct client* client = client_accept(server_fd);
    struct linked_list* slot = linked_list_select(connections);

    struct http_connection* conn = http_connection_alloc(client);
    slot->item = conn;

    return slot;
}

void http_connection_release(struct linked_list* slot) {
    if (slot->item != NULL)
        http_connection_close(slot->item);

    linked_list_release(slot);
}

void http_connection_close(struct http_connection* conn) {
    http_connection_free(conn);
}
