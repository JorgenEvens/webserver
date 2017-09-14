#include <string.h>
#include <unistd.h>

int buffer_pushback(char* buf, char* new_head, size_t buf_size) {
    char* end = buf + buf_size;
    size_t len = end - new_head;

    memcpy(buf, new_head, len);

    return new_head - buf;
};
