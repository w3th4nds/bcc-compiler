#ifndef _HASHMAP_H_
#define _HASHMAP_H_

typedef struct Scope_s Scope_t;

#include "debug.h"
#include "list.h"
#include "types.h"
#include "scope.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define HASHMAP_SZ  10

typedef struct Scope_s {
  char *scope_id;
  Type_t specs_type;
  // used in assembly generation phase
  // a leaf function does not call any other function
  bool is_leaf;
  List_t *params;
  Symtab_t *symtab;
  // linked list for hash collisions
  struct Scope_s *next;
} Scope_t;

int gethash(char *id);
Scope_t *create_scope(char *scope_id);
Scope_t **create_hashmap(void);
void hashmap_add(Scope_t **hashmap, char *scope_id, SymtabEntry_t *symtab_entry);
void hashmap_add_specs(Scope_t **hashmap, char *scope_id, Type_t type, List_t *params);
bool hashmap_key_exists(Scope_t **hashmap, char *scope_id);
void hashmap_set(Scope_t **hashmap, char *scope_id);
SymtabEntry_t *hashmap_getsymtabentry(Scope_t **hashmap, char *scope_id, char *entry_id);
Scope_t *hashmap_getscope(Scope_t **hashmap, char *scope_id);
List_t *hashmap_get_all_ids(Scope_t **hashmap);

#endif
