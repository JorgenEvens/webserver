#include <stdio.h>

#include "http_connection.h"
#include "http_request.h"
#include "client.h"
#include "utils.h"

void http_utils_echo(struct http_connection* conn) {
    int out_fd = conn->client->fd;
    struct http_request* req = &conn->req;

    push_str(out_fd, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\n");
    push_str(out_fd, req->method);
    push_str(out_fd, " ");
    push_str(out_fd, req->path);
    push_str(out_fd, " ");
    push_str(out_fd, req->protocol);
    push_str(out_fd, "\n\n");

    for (int i = 0; i < req->headers.count; i++) {
        struct http_header* h = req->headers.headers[i];

        if (h == NULL)
            continue;

        push_str(out_fd, req->headers.headers[i]->name);
        push_str(out_fd, ":");
        push_str(out_fd, req->headers.headers[i]->value);
        push_str(out_fd, "\n");
    }

}

