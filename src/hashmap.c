#include "include/hashmap.h"

int gethash(char *id)
{
  // index 0 reserved for global scope
  if (id == NULL) return 0;
  int idx = 0, acc = 0;
  while (id[idx] != '\0') acc += id[idx++];
  return (acc % (HASHMAP_SZ-1)) + 1;
}

Scope_t *create_scope(char *scope_id)
{
  Scope_t *scope = calloc(1, sizeof(Scope_t));
  scope->scope_id = scope_id;
  scope->specs_type = NO_TYPE;
  scope->is_leaf = true;
  scope->params = NULL;
  scope->symtab = init_symtab();
  return scope;
}

Scope_t **create_hashmap(void)
{
  Scope_t **hashmap = calloc(HASHMAP_SZ, sizeof(Scope_t *));
  hashmap[0] = create_scope(NULL);
  return hashmap;
}

// add a symbol table entry to current symtab
void hashmap_add(Scope_t **hashmap, char *scope_id, SymtabEntry_t *symtab_entry)
{
  int key = gethash(scope_id);
  Scope_t *scope = hashmap[key];
  if (key != 0) {
    for (;;) {
      if (scope == NULL) error_exit("hashmap_add() - scope = NULL, current scope id not found in hashmap\n");
      if (strcmp(scope->scope_id, scope_id) == 0) break;
      scope = scope->next;
    }
  }
  symtab_entry->offset = scope->symtab->offset + symtab_entry->size;
  // achieve alignment
  symtab_entry->offset += symtab_entry->offset % symtab_entry->size;
  scope->symtab->offset = symtab_entry->offset;
  list_push(scope->symtab->list, symtab_entry);
}

void hashmap_add_specs(Scope_t **hashmap, char *scope_id, Type_t type, List_t *params)
{
  int key = gethash(scope_id);
  Scope_t *scope = hashmap[key];
  if (key != 0) {
    for (;;) {
      if (scope == NULL) error_exit("hashmap_add_specs(): scope_id does not exist\n");;
      if (strcmp(scope->scope_id, scope_id) == 0) break;
      scope = scope->next;
    }
  }
  scope->specs_type = type;
  scope->params = params;
}

bool hashmap_key_exists(Scope_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  if (key == 0) return true;
  Scope_t *scope = hashmap[key];
  for (;;) {
    if (scope == NULL) break;
    if (strcmp(scope->scope_id, scope_id) == 0) return true;
    scope = scope->next;
  }
  return false;
}

// create a scope for a NEW scope
void hashmap_set(Scope_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  Scope_t *new_scope = create_scope(scope_id);
  if (hashmap[key] == NULL) {
    hashmap[key] = new_scope;
  }
  else {
    Scope_t *scope = hashmap[key];
    while (scope->next != NULL) scope = scope->next;
    scope->next = new_scope;
  }
}

SymtabEntry_t *hashmap_getsymtabentry(Scope_t **hashmap, char *scope_id, char *entry_id)
{
  Scope_t *scope = hashmap_getscope(hashmap, scope_id);
  SymtabEntry_t *entry = symtab_getentry(scope->symtab, entry_id);
  return entry;
}

Scope_t *hashmap_getscope(Scope_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  if (key == 0) return hashmap[0];
  Scope_t *scope = hashmap[key];
  for (;;) {
    if (scope == NULL) break;
    if (strcmp(scope->scope_id, scope_id) == 0) return scope;
    scope = scope->next;
  }
  return NULL;
}

List_t *hashmap_get_all_ids(Scope_t **hashmap)
{
  List_t *ids = init_list(sizeof(char *));
  for (int i = 1; i < HASHMAP_SZ; ++i) {
    Scope_t *scope = hashmap[i];
    while (scope != NULL) {
      list_push(ids, scope->scope_id);
      scope = scope->next;
    }
  }
  return ids;
}
