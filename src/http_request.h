#ifndef H_HTTP_REQUEST
#define H_HTTP_REQUEST

#include "http_headers.h"
#include "client.h"

struct http_connection;

struct http_request {
    char* method;
    char* protocol;
    char* path;

    struct http_headers headers;
    char* body;
};

void http_request_release(struct http_request* req);

void http_request_parse(struct client* client, struct http_connection* conn);

#endif
