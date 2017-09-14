#ifndef HTTP_STATE
#define HTTP_STATE

#define HTTP_STATE_BUF_SIZE 4096

typedef enum {
    S_METHOD,
    S_PATH,
    S_PROTO,
    S_HEADER_NAME,
    S_HEADER_VALUE,
    S_BODY,
    S_FINISHED,
    S_ERR = -1
} state_t;

struct http_state {
    state_t state;
    int size;
    char* head;
    char* tail;
    char buffer[HTTP_STATE_BUF_SIZE];
    void* handler_state;
};

void http_state_init(struct http_state* state);

#endif
