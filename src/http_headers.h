#ifndef H_HTTP_HEADER
#define H_HTTP_HEADER

struct http_header {
    char* name;
    char* value;
};

struct http_headers {
    struct http_header** headers;

    int count;
};

struct http_header* http_header_alloc(int name_len, int value_len);
struct http_header* http_header_realloc(struct http_header* header, int name_len, int value_len);
struct http_header* http_header_set(struct http_header* header, const char* name, const char* value);
void http_header_free(struct http_header* header);

void http_headers_grow(struct http_headers* headers);
void http_headers_add(struct http_headers* headers, char* name, char* value);
void http_headers_release(struct http_headers* headers);
void http_headers_free(struct http_headers* headers);

#endif
