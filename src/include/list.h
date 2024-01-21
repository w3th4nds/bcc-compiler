#ifndef _LIST_H_
#define _LIST_H_
#include <stdlib.h>

typedef struct {
  void **items;
  size_t item_size;
  size_t size;
} List_t;

List_t *init_list(size_t item_size);
void list_push(List_t *list, void *item);
int list_length(List_t *list);
void *list_getitem(List_t *list, int idx);
void list_pop_first(List_t *list);

#endif
