#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#include "http_connection.h"
#include "http_request.h"
#include "http_headers.h"
#include "client.h"
#include "buffer.h"

#ifdef DEBUG_HTTP_PARSER
    #define DEBUG(...) printf(__VA_ARGS__)
#else
    #define DEBUG(...)
#endif

#define FREE(v) if (v != NULL) {\
    free(v); \
    v = NULL; \
}


void http_request_release(struct http_request* req) {
    http_headers_release(&req->headers);

    FREE(req->method);
    FREE(req->path);
    FREE(req->protocol);
}

void state_pushback(struct http_state* state, char* from, state_t next) {
    state->head -= buffer_pushback(state->buffer, from, state->size);
    state->tail = state->buffer;
    state->state = next;
}

char* strread(char* source, int num) {
    char* buf = (char*)malloc(sizeof(char) * (num + 1));
    buf[num] = '\0';
    strncpy(buf, source, num);
    return buf;
}

void http_request_parse(struct client* client, struct http_connection* conn) {
    int socket_fd = client->fd;

    struct http_request* req = &conn->req;
    struct http_state* state = &conn->state;

    while(1) { // Loop until no more work

        if (state->state == S_BODY || state->state == S_ERR) {
            DEBUG("End of request reached.");
            return; // Should set return status
        }

        // Available write window
        int w_size = (state->buffer + state->size) - state->head;

        // Currently read window
        int r_size = (state->tail - state->buffer);

        // The last character
        char end = *state->tail;

        if (state->state == S_ERR)
            return;

        if (state->tail == state->head) {
            if (w_size < 1) {
                DEBUG("we are stuck with a window of %i\n", w_size);
                state->state = S_ERR;
                return;
            }

            DEBUG("Filling buffer with %i bytes\n", w_size);
            int bytes = read(socket_fd, state->head, w_size);
            if (bytes < 1)
                return;

            DEBUG("Was able to read %i bytes\n", bytes);

            state->head += bytes;
            continue;
        }

        if (state->state == S_METHOD && end == ' ') {
            req->method = strread(state->buffer, r_size);
            state_pushback(state, state->tail + 1, S_PATH);
            DEBUG("Found method %s\n", req->method);
            continue;
        }

        if (state->state == S_PATH && end == ' ') {
            req->path = strread(state->buffer, r_size);
            state_pushback(state, state->tail + 1, S_PROTO);
            DEBUG("Found path %s\n", req->path);
            continue;
        }

        if (state->state == S_PROTO && (end == ' ' || end == '\n')) {
            req->protocol = strread(state->buffer, r_size);
            state_pushback(state, state->tail + 1, S_HEADER_NAME);
            DEBUG("Found protocol %s\n", req->protocol);
            continue;
        }

        if (state->state == S_HEADER_NAME && end == ':') {
            *state->tail = '\0';
            state->state = S_HEADER_VALUE;
            continue;
        }

        if (state->state == S_HEADER_NAME && end == '\n') {
            state->state = S_BODY;
            DEBUG("Reached body\n");
            continue;
        }

        if (state->state == S_HEADER_VALUE && end == '\n') {
            *state->tail = '\0';
            int name_len = strlen(state->buffer);
            char* value_ptr = state->buffer + name_len + 2;

            DEBUG("Found headers: %s with value %s\n", state->buffer, value_ptr);
            http_headers_add(&req->headers, state->buffer, value_ptr);

            state_pushback(state, state->tail + 1, S_HEADER_NAME);
            continue;
        }

        state->tail++;
    }

    return;
}
