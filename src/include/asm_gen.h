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

typedef struct {
  AST_t *node;
  ScopeManager_t *scope_manager;
} AsmCtx_t;

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root);
char *asm_func_def(AsmCtx_t *ctx);
char *asm_return(AsmCtx_t *ctx);
char *asm_assignment(AsmCtx_t *ctx);

#endif
