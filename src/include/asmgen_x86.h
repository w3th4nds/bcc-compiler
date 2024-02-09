#ifndef _ASM_GEN_H_
#define _ASM_GEN_H_

#include "debug.h"
#include "util.h"
#include "AST.h"
#include "scope.h"
#include "hashmap.h"
#include "registers.h"
#include "macros.h"
#include <stdlib.h>
#include <assert.h>

#define STACK_REDZONE_SIZE  128

typedef struct {
  ScopeManager_t *scope_manager;
  RegisterManager_t *reg_manager;
} AsmCtx_t;

// if computed -> use value
// else -> use *code
typedef struct {
  bool computed;
  long value;
  char *code;
} BinopResult_t;

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root);
char *asm_access(AsmCtx_t *ctx, AST_t *node);
char *asm_func_def(AsmCtx_t *ctx, AST_t *node);
char *asm_statement(AsmCtx_t *ctx, AST_t *node);
char *asm_compound(AsmCtx_t *ctx, AST_t *node);
char *asm_return(AsmCtx_t *ctx, AST_t *node);
char *asm_assignment(AsmCtx_t *ctx, AST_t *node);
char *asm_call(AsmCtx_t *ctx, AST_t *node);
char *asm_while(AsmCtx_t *ctx, AST_t *node);
char *asm_for(AsmCtx_t *ctx, AST_t *node);
char *asm_condition(AsmCtx_t *ctx, AST_t *node, char *jmp_label);

BinopResult_t *binop_evaluate(AsmCtx_t *ctx, AST_t *node);
bool binop_iscomputable(AST_t *node);
long binop_evaluate_(AST_t *node);
void binop_gen_code(AsmCtx_t *ctx, AST_t *node, BinopResult_t *res);

char *make_label(void);

#endif
