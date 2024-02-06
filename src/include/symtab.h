#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "types.h"
#include "list.h"
#include <stdio.h>

typedef struct {
  Type type;
  size_t size;
  size_t offset;
  char *name;
  char *scope_id;
} SymtabEntry_t;

typedef struct {
  unsigned int offset;
  List_t *list;
} Symtab_t;

Symtab_t *init_symtab(void);
SymtabEntry_t *init_symtab_entry(Type type, char *name, char *scope_id);
SymtabEntry_t *symtab_getentry(Symtab_t *symtab, char *entry_id);
void print_symtab(Symtab_t *symtab);

#endif
