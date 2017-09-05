#include <stdio.h>
#include <stdlib.h>

#include "circular_buffer.h"

struct circular_buffer* circular_buffer_realloc(struct circular_buffer* buf, int size) {
    if (buf == NULL)
        return circular_buffer_alloc(size);

    if (buf->buffer != NULL)
        free(buf->buffer);

    buf->size = size;
    buf->head = 0;
    buf->tail = 0;
    buf->buffer = (char*)malloc(sizeof(char) * (size + 1));

    return buf;
}

struct circular_buffer* circular_buffer_alloc(int size) {
    struct circular_buffer* buf = (struct circular_buffer*)malloc(sizeof(struct circular_buffer));

    return circular_buffer_realloc(buf, size);
}

void circular_buffer_free(struct circular_buffer* buf) {
    if (buf->buffer != NULL)
        free(buf->buffer);

    buf->buffer = NULL;
    free(buf);
}

int circular_buffer_fill(struct circular_buffer* buf) {
    int head = buf->head;
    int tail = buf->tail;

    if (tail > head)
        return buf->size - ( tail - head );

    return head - tail;
}

int circular_buffer_remaining(struct circular_buffer* buf) {
    return buf->size - circular_buffer_fill(buf);
}

int circular_buffer_full(struct circular_buffer* buf) {
    return (buf->head + 1) % (buf->size + 1) == buf->tail;
}

int circular_buffer_empty(struct circular_buffer* buf) {
    return buf->head == buf->tail;
}
