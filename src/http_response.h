#ifndef H_HTTP_RESPONSE
#define H_HTTP_RESPONSE

#include <stdio.h>

#include "http_headers.h"

struct http_response {
    struct http_headers headers;
    char* body;
    FILE* file;
};

#endif
