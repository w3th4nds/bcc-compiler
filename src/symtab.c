#include "include/symtab.h"

List_t *init_symtab(void)
{
  List_t *symtab = init_list(sizeof(SymtabEntry_t *));
  return symtab;
}

SymtabEntry_t *init_symtab_entry(int type, char *name, char *scope_id)
{
  SymtabEntry_t *symtab_entry = calloc(1, sizeof(SymtabEntry_t));
  symtab_entry->type = type;
  symtab_entry->name = name;
  symtab_entry->scope_id = scope_id;
  return symtab_entry;
}

void print_symtab(List_t *symtab)
{
  printf("#########################################################\n");
  for (int i = 0; i < symtab->size; ++i) {
    SymtabEntry_t *entry = symtab->items[i];
    printf("[\t%s\t|\t%s\t|\t%s\t]\n",
           entry->name,
           type_to_str(entry->type),
           entry->scope_id);
  }
}
