#ifndef H_BUFFER
#define H_BUFFER

#include <unistd.h>

int buffer_pushback(char* buf, char* new_head, size_t buf_size);

#endif
