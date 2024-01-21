#include "include/scope.h"

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

void scope_add_specs(ScopeManager_t *scope_manager, int type, List_t *params)
{
  hashmap_add_specs(scope_manager->scopes, scope_manager->current_scope_id, type, params);
}

void print_scopes(ScopeManager_t *scope_manager)
{
  for (int i = 0; i < HASHMAP_SZ; ++i) {
    Bucket_t *bucket = scope_manager->scopes[i];
    while (bucket != NULL) {
      if (bucket->scope_id == NULL) {
        printf("\nGlobal scope:\n");
      }
      else {
        printf("\n%s:\n"
               "Return type: %s\n"
               "Params: (", bucket->scope_id, type_to_str(bucket->specs_type));
        for (int ii = 0; ii < bucket->params->size; ++ii) {
          AST_t *param = bucket->params->items[ii];
          printf("%s %s", type_to_str(param->specs_type), param->name);
          if (ii != bucket->params->size-1) printf(", ");
        }
        printf(")\n");
      }
      printf("Symtab:\n");
      print_symtab(bucket->symtab);
      bucket = bucket->next;
      printf("---\n");
    }
  }
}
