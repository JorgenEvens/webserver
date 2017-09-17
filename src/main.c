#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "linked_list.h"
#include "http_state.h"
#include "http_headers.h"
#include "http_request.h"
#include "http_connection.h"
#include "utils.h"
#include "file.h"
#include "http_utils.h"
#include "http_route.h"

char* public_directory = "public";

void respond_echo(struct http_connection* conn) {
    http_utils_echo(conn);
    conn->state.state = S_FINISHED;
}

void respond_file(struct http_connection* conn) {
    int socket_fd = conn->client->fd;
    struct http_request* req = &conn->req;

    FILE* f = (FILE*)conn->state.handler_state;
    FILE** f_ptr = (FILE**)&conn->state.handler_state;

    if (f == NULL) {
        f = http_file_open(req->path, public_directory);
        *f_ptr = f;
        printf("[request] %s %s %s\n", req->method, req->path, req->protocol);

        if (f == NULL) {
            printf("Failed to open file %s\n", req->path);
            push_str(socket_fd, "HTTP/1.0 404 Not Found\n\n");
            conn->state.state = S_ERR;
        } else {
            push_str(socket_fd, "HTTP/1.0 200 OK\nConnection: close\n\n");
        }
        return;
    }

    if (feof(f)) {
        fclose(f);
        *f_ptr = NULL;
        conn->state.state = S_FINISHED;
        return;
    }

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    size_t bytes = fread(buf, sizeof(char), 1024, f);

    if (write(socket_fd, buf, bytes) < 0) {
        fclose(f);
        *f_ptr = NULL;
        conn->state.state = S_ERR;
        return;
    }
}

int select_all_connections(struct linked_list* connections, fd_set* set) {
    int fd_max = 0;
    do {
        struct http_connection* conn = connections->item;
        connections = connections->next;

        if (conn == NULL)
            continue;

        FD_SET(conn->client->fd, set);

        if (conn->client->fd > fd_max)
            fd_max = conn->client->fd;

    } while(connections != NULL);

    return fd_max;
}

void select_handle_read(struct linked_list* connections, fd_set* set) {
    do {
        struct http_connection* conn = connections->item;
        connections = connections->next;

        if (conn == NULL)
            continue;

        state_t s = conn->state.state;
        if (s == S_BODY || s == S_ERR)
            continue;

        if (!FD_ISSET(conn->client->fd, set))
            continue;

        http_request_parse(conn->client, conn);
    } while (connections != NULL);
}

void set_handler(struct linked_list* connections) {
    do {
        struct http_connection* conn = connections->item;
        connections = connections->next;

        if (conn == NULL)
            continue;

        if (conn->state.handler != NULL)
            continue;

        state_t s = conn->state.state;
        if ((s != S_BODY && s != S_FINISHED) || s == S_ERR)
            continue;

        http_route_set(conn, "/echo", &respond_echo);
        http_route_default(conn, &respond_file);

    } while (connections != NULL);
}

void select_handle_write(struct linked_list* connections, fd_set* set) {
    do {
        struct http_connection* conn = connections->item;
        struct linked_list* slot = connections;
        connections = connections->next;

        if (conn == NULL)
            continue;

        state_t s = conn->state.state;
        if ((s != S_BODY && s != S_FINISHED) || s == S_ERR)
            continue;

        int socket_fd = conn->client->fd;
        if (!FD_ISSET(socket_fd, set))
            continue;

        conn->state.handler(conn);

        if (conn->state.state == S_FINISHED || conn->state.state == S_ERR) {
            char* end_seq = "\n\n";
            write(socket_fd, end_seq, strlen(end_seq));
            close(socket_fd);

            http_connection_release(slot);
            linked_list_free(slot);
        }
    } while (connections != NULL);
}

int main(int argc, char** argv) {
    printf("Program received %i arguments\n", argc);

    if (argc < 3) {
        printf("Please specify port number and public dir");
        return 1;
    }

    // Disable signals on failed socket writes
    signal(SIGPIPE, SIG_IGN);

    int server_fd = server_listen(argv[1], NULL);
    public_directory = argv[2];

    printf("Listening with FD %i\n", server_fd);

    fd_set fd_read;
    fd_set fd_write;

    struct linked_list* connections = linked_list_select(NULL);
    struct http_connection* conn = NULL;
    struct linked_list* slot = NULL;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    while (1) {
        FD_ZERO(&fd_read);
        FD_ZERO(&fd_write);

        // enable monitoring on connections
        FD_SET(server_fd, &fd_read);
        select_all_connections(connections, &fd_read);
        int fd_max = select_all_connections(connections, &fd_write);

        if (server_fd > fd_max)
            fd_max = server_fd;

        int ret = select(fd_max + 1, &fd_read, &fd_write, NULL, &tv);

        if (ret < 0) {
            printf("Error in select()\n");
            continue;
        }

        if (ret == 0) {
            continue;
        }

        if (FD_ISSET(server_fd, &fd_read)) {
            printf("Connection available %i\n", linked_list_length(connections));
            slot = http_connection_accept(server_fd, connections);
            conn = slot->item;
            http_state_init(&conn->state);
            continue;
        }

        select_handle_read(connections, &fd_read);
        set_handler(connections);
        select_handle_write(connections, &fd_write);
    }

    close(server_fd);
    free(connections);

    return 0;
}
