#include "include/scope.h"

/*

  Scope Manager structure:
  --
  A hashmap holds the symtab for each given scope
  "current_scope_id" points to the function id currently being parsed

  scopes are the hashmap entries that hold:
  - the id (function name)
  - the function return type
  - the function parameters and their types
  - the scope's symtab

  A Symtab is a list of SymtabEntries, and each of them holds:
  - the variable type
  - the variable's size in bytes
  - the variable's offset on the stack (or the .data section for globals)
  - the variable's name
  - the variable's scope (even though not necessary so far)

  The symtab also accounts for stack alignment.

  A linked list is built in case of hash collisions
  Index 0 of the hashmap is reserved for the global scope

*/

ScopeManager_t *init_scope_manager(void)
{
  ScopeManager_t *scope_manager = calloc(1, sizeof(ScopeManager_t));
  scope_manager->scopes = create_hashmap();
  return scope_manager;
}

// create/set scope
void scope_set(ScopeManager_t *scope_manager, char *scope_id)
{
  if (SCOPE_DEBUG) {
    if (scope_id == NULL) printf("Setting scope -> global\n");
    else printf("Setting scope -> \"%s\"\n", scope_id);
  }
  scope_manager->current_scope_id = scope_id;
  if (!hashmap_key_exists(scope_manager->scopes, scope_id))
    hashmap_set(scope_manager->scopes, scope_id);
}

// add symbol to CURRENT symbol table
void scope_add_to_symtab(ScopeManager_t *scope_manager, int type, char *name)
{
  if (SCOPE_DEBUG) {
    if (scope_manager->current_scope_id == NULL) printf("Current scope: global");
    else printf("Current_scope: %s", scope_manager->current_scope_id);
    printf(" - adding: \"%s\" %s\n", name, type_to_str(type));
  }
  SymtabEntry_t *symtab_entry = init_symtab_entry(type, name, scope_manager->current_scope_id);
  hashmap_add(scope_manager->scopes, scope_manager->current_scope_id, symtab_entry);
}

// specify function return type
// and the parameters the current function
void scope_add_specs(ScopeManager_t *scope_manager, int type, List_t *params)
{
  if (hashmap_key_exists(scope_manager->scopes, scope_manager->current_scope_id))
    hashmap_add_specs(scope_manager->scopes, scope_manager->current_scope_id, type, params);
  else 
    error_exit("scope_add_specs() - current scope does not exist in hashmap\n");
}

Scope_t *scope_getcurrentscope(ScopeManager_t *scope_manager)
{
  Scope_t *scope = hashmap_getscope(scope_manager->scopes, scope_manager->current_scope_id);
  return scope;
}

// search for an entry id in the current scope
SymtabEntry_t *scope_getsymtabentry(ScopeManager_t *scope_manager, char *entry_id)
{
  SymtabEntry_t *entry = hashmap_getsymtabentry(scope_manager->scopes, scope_manager->current_scope_id, entry_id);
  return entry;
}

void print_scopes(ScopeManager_t *scope_manager)
{
  for (int i = 0; i < HASHMAP_SZ; ++i) {
    Scope_t *scope = scope_manager->scopes[i];
    while (scope != NULL) {
      if (scope->scope_id == NULL) {
        printf("\nGlobal scope:\n");
      }
      else {
        printf("\n%s:\n"
               "Return type: %s\n"
               "Params: (", scope->scope_id, type_to_str(scope->specs_type));
        for (int ii = 0; ii < scope->params->size; ++ii) {
          AST_t *param = scope->params->items[ii];
          printf("%s %s", type_to_str(param->specs_type), param->name);
          if (ii != scope->params->size-1) printf(", ");
        }
        printf(")\n");
      }
      printf("Symtab:\n");
      print_symtab(scope->symtab);
      scope = scope->next;
      printf("---\n");
    }
  }
}
