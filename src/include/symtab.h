#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "types.h"
#include "list.h"
#include <stdio.h>

typedef struct {
  int type;
  char *name;
  char *scope_id;
} SymtabEntry_t;

List_t *init_symtab(void);
SymtabEntry_t *init_symtab_entry(int type, char *name, char *scope_id);
void print_symtab(List_t *symtab);

#endif
