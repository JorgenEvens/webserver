#include <stdio.h>
#include <string.h>

#include "http_route.h"
#include "http_request.h"
#include "http_connection.h"

void http_route_set(struct http_connection* conn, const char* path, void (*handler)(struct http_connection*)) {
    if (conn->state.handler != NULL)
        return;

    if (strcmp(path, conn->req.path) != 0)
        return;

    conn->state.handler = handler;
}

void http_route_default(struct http_connection* conn, void (*handler)(struct http_connection*)) {
    if (conn->state.handler != NULL)
        return;

    conn->state.handler = handler;
}
