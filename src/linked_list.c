#include <string.h>
#include <stdlib.h>
#include "linked_list.h"

struct linked_list* linked_list_add(struct linked_list* slot, void* item) {
    struct linked_list* add = (struct linked_list*)malloc(sizeof(struct linked_list));
    memset(add, 0, sizeof(struct linked_list));

    add->item = item;

    if (slot == NULL)
        return add;

    struct linked_list* next = slot->next;

    if (next != NULL)
        next->prev = add;

    slot->next = add;
    add->prev = slot;
    add->next = next;

    return add;
}

int linked_list_release(struct linked_list* slot) {
    if (slot == NULL)
        return -1;

    slot->item = NULL;

    return 0;
}

int linked_list_free(struct linked_list* slot) {
    if (slot == NULL)
        return -1;

    linked_list_release(slot);

    struct linked_list* next = slot->next;
    struct linked_list* prev = slot->prev;

    // Cannot release head
    if (prev == NULL)
        return 0;

    prev->next = next;

    if (next != NULL)
        next->prev = prev;

    free(slot);

    return 0;
}

struct linked_list* linked_list_create() {
    return linked_list_select(NULL);
}

struct linked_list* linked_list_select(struct linked_list* list) {
    if (list == NULL)
        return linked_list_add(NULL, NULL);

    while (list->item != NULL && list->next != NULL)
        list = list->next;

    if (list->item != NULL && list->next == NULL)
        list = linked_list_add(list, NULL);

    return list;
}

int linked_list_length(struct linked_list* list) {
    int counter = 0;
    while(list != NULL) {
        counter += 1;
        list = list->next;
    }
    return counter;
}
