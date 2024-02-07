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
void scope_add_to_symtab(ScopeManager_t *scope_manager, Type_t type, char *name)
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
// and the parameters of the current function
void scope_add_specs(ScopeManager_t *scope_manager, Type_t type, List_t *params)
{
  if (hashmap_key_exists(scope_manager->scopes, scope_manager->current_scope_id))
    hashmap_add_specs(scope_manager->scopes, scope_manager->current_scope_id, type, params);
  else 
    error_exit("scope_add_specs() - current scope does not exist in hashmap\n");
}

void scope_unset_leaf(ScopeManager_t *scope_manager)
{
  if (scope_manager->current_scope_id == NULL) return;
  Scope_t *scope = hashmap_getscope(scope_manager->scopes, scope_manager->current_scope_id);
  scope->is_leaf = false;
}

Scope_t *scope_getcurrentscope(ScopeManager_t *scope_manager)
{
  assert(scope_manager->current_scope_id != NULL && "scope_getcurrentscope() - current_scope_id is NULL\n");
  Scope_t *scope = hashmap_getscope(scope_manager->scopes, scope_manager->current_scope_id);
  return scope;
}

Scope_t *scope_getscopebyid(ScopeManager_t *scope_manager, char *scope_id)
{
  assert(scope_id != NULL && "scope_getscopebyid() - scope_id is NULL\n");
  Scope_t *scope = hashmap_getscope(scope_manager->scopes, scope_id);
  return scope;
}

Variable_t *scope_getvariable(ScopeManager_t *scope_manager, char *var_id)
{
  assert(var_id != NULL && "scope_getvariable() - var_id is NULL\n");
  Variable_t *var = calloc(1, sizeof(Variable_t));
  // Search locally
  SymtabEntry_t *entry = hashmap_getsymtabentry(scope_manager->scopes, scope_manager->current_scope_id, var_id);
  if (entry != NULL) {
    var->entry = entry;
    var->offset = entry->offset;
    var->size = entry->size;
    var->scope = VAR_LOCAL;
    return var;
  }
  // Search params
  Scope_t *scope = scope_getcurrentscope(scope_manager);
  for (int i = 0; i < scope->params->size; ++i) {
    AST_t *param = scope->params->items[i];
    if (param->name != NULL && strcmp(var_id, param->name) == 0) {
      var->decl = param;
      var->param_n = i;
      var->size = type_size(param->specs_type);
      var->scope = VAR_PARAM;
      return var;
    }
  }
  // Search global
  entry = hashmap_getsymtabentry(scope_manager->scopes, NULL, var_id);
  if (entry != NULL) {
    var->entry = entry;
    var->offset = entry->offset;
    var->size = entry->size;
    var->scope = VAR_GLOBAL;
    return var;
  }
  error_exit("scope_getvariable() - var_id could not be found\n");
  return NULL;
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
      printf("is_leaf: %s\n", scope->is_leaf ? "true" : "false");
      printf("Symtab:\n");
      print_symtab(scope->symtab);
      scope = scope->next;
      printf("---\n");
    }
  }
}
