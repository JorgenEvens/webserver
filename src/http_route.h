#ifndef H_HTTP_ROUTE
#define H_HTTP_ROUTE

struct http_connection;

void http_route_set(struct http_connection* conn, const char* path, void (*handler)(struct http_connection*));
void http_route_default(struct http_connection* conn, void (*handler)(struct http_connection*));

#endif
