#include <stdio.h>

#include "http_headers.h"

#ifndef H_HTTP_RESPONSE
#define H_HTTP_RESPONSE

struct http_response {
    struct http_headers headers;
    char* body;
    FILE* file;
};

#endif
