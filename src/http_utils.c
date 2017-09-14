#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

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

FILE* http_file_open(char* relpath, char* relativeTo) {
    int len = strlen(relpath) + strlen(relativeTo) + 3;
    char path[len];
    snprintf(path, len, "./%s%s", relativeTo, relpath);

    struct stat pathinfo;
    if (stat(path, &pathinfo) != 0)
        return NULL;

    if (S_IFDIR == (pathinfo.st_mode & S_IFDIR)) {
        len = strlen(relpath) + strlen("/index.html") + 1;
        char indexPath[len];
        snprintf(indexPath, len, "%s/index.html", relpath);
        return http_file_open(indexPath, relativeTo);
    }

    return fopen(path, "r");
}
