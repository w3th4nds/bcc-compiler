#include "include/list.h"

List_t *init_list(size_t item_size)
{
  List_t *list = calloc(1, sizeof(List_t));
  list->items = NULL;
  list->item_size = item_size;
  list->size = 0;
  return list;
}

void list_push(List_t *list, void *item)
{
  list->size++;
  if (list->items == NULL) list->items = calloc(1, list->item_size);
  else list->items = realloc(list->items, list->size * list->item_size);
  list->items[list->size-1] = item;
}

void list_pop_first(List_t *list)
{
  if (list == NULL || list->items == NULL) return;
  for (int i = 0; i < list->size-1; ++i) list->items[i] = list->items[i+1];
  list->size--;
}
