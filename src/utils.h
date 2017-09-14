#ifndef H_UTILS
#define H_UTILS

#include <unistd.h>
#include <stdio.h>

int fail_with_error(const char* message, int exit_code);

void push_str(int out_fd, char* str);

int buf_pushback(char* buf, char* new_head, size_t buf_size);

#endif
