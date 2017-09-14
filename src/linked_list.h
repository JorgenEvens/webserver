#ifndef H_LINKED_LIST
#define H_LINKED_LIST

struct linked_list {
    void* item;
    struct linked_list* prev;
    struct linked_list* next;
};

struct linked_list* linked_list_create();
struct linked_list* linked_list_add(struct linked_list* slot, void* item);
struct linked_list* linked_list_select(struct linked_list* list);

int linked_list_length(struct linked_list* list);

int linked_list_release(struct linked_list* slot);
int linked_list_free(struct linked_list* slot);

#endif
