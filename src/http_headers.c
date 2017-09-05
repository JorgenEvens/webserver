#include <string.h>
#include <stdlib.h>

#include "http_headers.h"

#define HTTP_HEADER_INITIAL_COUNT 8

struct http_header* http_header_alloc(int name_len, int value_len) {
    struct http_header* header = (struct http_header*)malloc(sizeof(struct http_header));

    header->name = NULL;
    header->value = NULL;

    return http_header_realloc(header, name_len, value_len);
}

struct http_header* http_header_realloc(struct http_header* header, int name_len, int value_len) {
    if (header == NULL)
        return http_header_alloc(name_len, value_len);

    // Free old stringdata
    if (header->name != NULL)
        free(header->name);

    int len = name_len + value_len + 2;

    char* strings = (char*)malloc(sizeof(char) * len);
    memset(strings, 0, sizeof(char) * len);

    header->name = strings;
    header->value = strings + name_len + 1;

    return header;
}

void http_header_free(struct http_header* header) {
    if (header->name != NULL)
        free(header->name);

    free(header);
}

struct http_header* http_header_set(struct http_header* header, const char* name, const char* value) {
    int name_len = strlen(name);
    int value_len = strlen(value);

    header = http_header_realloc(header, name_len, value_len);

    strncpy(header->name, name, name_len);
    strncpy(header->value, value, value_len);

    return header;
}

void http_headers_grow(struct http_headers* headers) {
    int count = headers->count;
    int next_count = count < 1 ? HTTP_HEADER_INITIAL_COUNT : count * 2;

    struct http_header** header_list = (struct http_header**)malloc(sizeof(struct http_header*) * next_count);
    memset(header_list, 0, sizeof(struct http_header*) * next_count);

    if (count > 0)
        memcpy(header_list, headers->headers, sizeof(struct http_header*) * count);

    headers->headers = header_list;
    headers->count = next_count;
}

void http_headers_add(struct http_headers* headers, char* name, char* value) {
    for (int i = 0; i < headers->count; i++) {
        if (headers->headers[i] != NULL)
            continue;

        headers->headers[i] = http_header_set(NULL, name, value);
        return;
    }

    http_headers_grow(headers);

    return http_headers_add(headers, name, value);
}

void http_headers_release(struct http_headers* headers) {
    for (int i = 0; i < headers->count; i++) {
        if (headers->headers[i] == NULL)
            continue;

        http_header_free(headers->headers[i]);
    }

    free(headers->headers);
    headers->headers = NULL;
    headers->count = 0;
}

void http_headers_free(struct http_headers* headers) {
    http_headers_release(headers);
    free(headers);
}
