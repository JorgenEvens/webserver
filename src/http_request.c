#include "http_request.h"
#include "http_headers.h"

void http_request_release(struct http_request* req) {
    http_headers_release(&req->headers);
}
