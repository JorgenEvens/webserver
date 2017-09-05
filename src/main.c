#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>

#include "client.h"
#include "server.h"
#include "linked_list.h"
#include "http_headers.h"
#include "http_connection.h"
#include "utils.h"
#include "http_utils.h"

#define BUF_SIZE 2048
#define S_METHOD 1
#define S_PATH 2
#define S_PROTO 3
#define S_HEADER_NAME 4
#define S_HEADER_VALUE 5
#define S_BODY 6
#define S_ERR -1

int buf_pushback(char* buf, char* new_head, size_t buf_size) {
    char* end = buf + buf_size;
    size_t len = end - new_head;

    memcpy(buf, new_head, len);

    return new_head - buf;
};


void parse_request(struct http_connection* conn) {
    char data[BUF_SIZE];
    memset(data, 0, BUF_SIZE);

    struct http_request* req = &conn->req;

    int state = S_METHOD;
    int socket_fd = conn->client->fd;
    char* start = data;
    char* end = data;
    char* fill = data;

    while (state != S_BODY && state != S_ERR) {
        int w_size = (data + BUF_SIZE) - fill;

        if (end >= fill) {
            printf("Filling buffer with %i bytes\n", w_size);
            int bytes = read(socket_fd, data, w_size);
            fill += bytes;
            continue;
        }

        if (state == S_METHOD && *end == ' ') {
            req->method = (char *)malloc(end - start + 1);
            snprintf(req->method, end - start + 1, "%s", start);
            fill -= buf_pushback(data, end + 1, BUF_SIZE);
            start = data;
            end = start;
            state = S_PATH;
        }

        if (state == S_PATH && *end == ' ') {
            req->path = (char *)malloc(end - start + 1);
            snprintf(req->path, end - start + 1, "%s", start);
            fill -= buf_pushback(data, end + 1, BUF_SIZE);
            start = data;
            end = start;
            state = S_PROTO;
        }

        if (state == S_PROTO && (*end == ' ' || *end == '\n')) {
            req->protocol = (char *)malloc(end - start + 1);
            snprintf(req->protocol, end - start + 1, "%s", start);
            fill -= buf_pushback(data, end + 1, BUF_SIZE);
            start = data;
            end = start;
            state = S_HEADER_NAME;
        }

        if (state == S_HEADER_NAME && *end == ':') {
            *end = '\0';
            start = end + 1;
            end = start;
            state = S_HEADER_VALUE;
        }

        if (state == S_HEADER_NAME && *end == '\n') {
            state = S_BODY;
        }

        if (state == S_HEADER_VALUE && *end == '\n') {
            *end = '\0';
            http_headers_add(&req->headers, data, start);

            // We currently do not persist these since we have no interest for them
            /* printf("Found header: %s = %s\n", data, start); */
            fill -= buf_pushback(data, end + 1, BUF_SIZE);
            start = data;
            end = start;
            state = S_HEADER_NAME;
        }

        end++;
    }
}

FILE* open_file(char* relpath, char* relativeTo) {
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
        return open_file(indexPath, relativeTo);
    }

    return fopen(path, "r");
}

void pipe_file(int out_fd, FILE* fd) {
    char buf[4096];

    size_t size = sizeof(char);
    int count = sizeof(buf) / size;

    while (feof(fd) == 0) {
        size_t bytes = fread(buf, size, count, fd);

        write(out_fd, buf, bytes);
    }
}


int main(int argc, char** argv) {
    printf("Program received %i arguments\n", argc);

    if (argc < 3) {
        printf("Please specify port number and public dir");
        return 1;
    }

    int server_fd = server_listen(argv[1], NULL);

    printf("Listening with FD %i\n", server_fd);

    struct linked_list* connections = linked_list_select(NULL);
    struct http_connection* conn = NULL;
    struct linked_list* slot = NULL;
    int socket_fd = -1;

    while (1) {
        slot = http_connection_accept(server_fd, connections);
        conn = slot->item;
        socket_fd = conn->client->fd;

        printf("Received connection\n");

        parse_request(conn);
        struct http_request* req = &conn->req;

        printf("%s %s %s\n", req->method, req->path, req->protocol);

        if (strcmp(req->path,"/echo") == 0) {
            http_utils_echo(conn);
        } else {
            FILE* f = open_file(req->path, argv[2]);

            if (f == NULL) {
                printf("Failed to open file %s\n", req->path);
                push_str(socket_fd, "HTTP/1.0 404 Not Found\n\n");
            } else {
                printf("Piping file %s\n", req->path);
                push_str(socket_fd, "HTTP/1.0 200 OK\nConnection: close\n\n");
                pipe_file(socket_fd, f);
                fclose(f);
            }
        }


        char* end_seq = "\n\n";
        write(socket_fd, end_seq, strlen(end_seq));

        http_connection_release(slot);
    }

    return 0;
}
