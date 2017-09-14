#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void file_pipe(int out_fd, FILE* fd) {
    char buf[4096];

    size_t size = sizeof(char);
    int count = sizeof(buf) / size;

    while (feof(fd) == 0) {
        size_t bytes = fread(buf, size, count, fd);

        write(out_fd, buf, bytes);
    }
}
