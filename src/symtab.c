#include "include/symtab.h"

SymtabEntry_t *init_symtab_entry(Type type, char *name, char *scope_id)
{
  SymtabEntry_t *symtab_entry = calloc(1, sizeof(SymtabEntry_t));
  symtab_entry->type = type;
  symtab_entry->size = type_size(type);
  symtab_entry->offset = 0;
  symtab_entry->name = name;
  symtab_entry->scope_id = scope_id;
  return symtab_entry;
}

Symtab_t *init_symtab(void)
{
  Symtab_t *symtab = calloc(1, sizeof(Symtab_t));
  symtab->offset = 0;
  symtab->list = init_list(sizeof(SymtabEntry_t *));
  return symtab;
}

SymtabEntry_t *symtab_getentry(Symtab_t *symtab, char *entry_id)
{
  for (int i = 0; i < symtab->list->size; ++i) {
    SymtabEntry_t *entry = symtab->list->items[i];
    if (strcmp(entry->name, entry_id) == 0) return entry;
  }
  //error_exit("symtab_getentry() - could not find the entry id requested\n");
  return NULL;
}

void print_symtab(Symtab_t *symtab)
{
  int pad = 0;
  for (int i = 0; i < symtab->list->size; ++i) {
    for (int j = 0; j < pad; ++j) putchar('-');
    putchar('\n');
    SymtabEntry_t *entry = symtab->list->items[i];
    printf("[ %-15s | %-12s | sz: %ld | off: 0x%-2lx ]%n\n",
           entry->name,
           type_to_str(entry->type),
           entry->size,
           entry->offset,
           &pad);
  }
}
