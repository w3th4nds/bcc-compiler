#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "debug.h"
#include "types.h"
#include "AST.h"
#include "symtab.h"
#include "hashmap.h"
#include <assert.h>

// used in asm_gen phase
typedef struct {
  enum {
    VAR_LOCAL,
    VAR_PARAM,
    VAR_GLOBAL
  } scope;
  union {
    SymtabEntry_t *entry;
    AST_t *decl;
  };
  union {
    int offset;
    int param_n;
  };
  size_t size;
} Variable_t;

typedef struct {
  Scope_t **scopes;
  char *current_scope_id;
} ScopeManager_t;

ScopeManager_t *init_scope_manager(void);
void scope_set(ScopeManager_t *scope_manager, char *scope_id);
void scope_add_to_symtab(ScopeManager_t *scope_manager, int type, char *name);
void scope_add_specs(ScopeManager_t *scope_manager, int type, List_t *params);
void scope_unset_leaf(ScopeManager_t *scope_manager);
Scope_t *scope_getcurrentscope(ScopeManager_t *scope_manager);
Scope_t *scope_getscopebyid(ScopeManager_t *scope_manager, char *scope_id);
Variable_t *scope_getvariable(ScopeManager_t *scope_manager, char *var_id);
void print_scopes(ScopeManager_t *scope_manager);

#endif
