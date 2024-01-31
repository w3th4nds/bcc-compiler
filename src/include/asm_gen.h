#ifndef _ASM_GEN_H_
#define _ASM_GEN_H_

#include "debug.h"
#include "AST.h"
#include "scope.h"
#include "hashmap.h"
#include "binop_eval.h"
#include "asm_templates/return_template.h"
#include "asm_templates/funcdef_template.h"
#include <stdlib.h>

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root);
char *asm_func_def(ScopeManager_t *scope_manager, AST_t *node);
char *asm_return(ScopeManager_t *scope_manager, AST_t *node);
char *asm_assignment(ScopeManager_t *scope_manager, AST_t *node);

#endif
