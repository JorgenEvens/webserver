#include "http_headers.h"

#ifndef H_HTTP_REQUEST
#define H_HTTP_REQUEST

struct http_request {
    char* method;
    char* protocol;
    char* path;

    struct http_headers headers;
    char* body;
};

void http_request_release(struct http_request* req);

#endif
