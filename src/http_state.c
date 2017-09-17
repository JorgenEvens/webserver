#include <string.h>

#include "http_state.h"

void http_state_init(struct http_state* state) {
    memset(state, 0, sizeof(struct http_state));
    state->state = S_METHOD;
    state->size = HTTP_STATE_BUF_SIZE;
    state->head = state->buffer;
    state->tail = state->buffer;
    state->handler = NULL;
    state->handler_state = NULL;
}
