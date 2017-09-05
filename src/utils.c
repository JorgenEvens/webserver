#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int fail_with_error(const char* message, int exit_code) {
    printf(message, strerror(errno));
    return exit_code;
}

void push_str(int out_fd, char* str) {
    write(out_fd, str, strlen(str));
}
