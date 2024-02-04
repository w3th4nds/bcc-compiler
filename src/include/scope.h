#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "debug.h"
#include "types.h"
#include "AST.h"
#include "symtab.h"
#include "hashmap.h"
#include <assert.h>

typedef struct {
  Scope_t **scopes;
  char *current_scope_id;
} ScopeManager_t;

ScopeManager_t *init_scope_manager(void);
void scope_set(ScopeManager_t *scope_manager, char *scope_id);
void scope_add_to_symtab(ScopeManager_t *scope_manager, int type, char *name);
void scope_add_specs(ScopeManager_t *scope_manager, int type, List_t *params);
Scope_t *scope_getcurrentscope(ScopeManager_t *scope_manager);
Scope_t *scope_getscopebyid(ScopeManager_t *scope_manager, char *scope_id);
SymtabEntry_t *scope_getsymtabentry(ScopeManager_t *scope_manager, char *entry_id);
void print_scopes(ScopeManager_t *scope_manager);

#endif
