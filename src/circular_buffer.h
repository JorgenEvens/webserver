
#ifndef H_CIRCULAR_BUFFER
#define H_CIRCULAR_BUFFER

struct circular_buffer {
    int head;
    int tail;
    int size;
    char* buffer;
};

struct circular_buffer* circular_buffer_alloc(int size);
struct circular_buffer* circular_buffer_realloc(struct circular_buffer* buf, int size);

void circular_buffer_free(struct circular_buffer* buf);

int circular_buffer_fill(struct circular_buffer* buf);
int circular_buffer_remaining(struct circular_buffer* buf);

int circular_buffer_full(struct circular_buffer* buf);
int circular_buffer_empty(struct circular_buffer* buf);

#endif
