#ifndef H_HTTP_UTILS
#define H_HTTP_UTILS

#include <stdio.h>

void http_utils_echo(struct http_connection* conn);

FILE* http_file_open(char* relpath, char* relativeTo);

#endif
