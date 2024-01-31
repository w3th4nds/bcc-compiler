#ifndef _BINOP_EVAL_H_
#define _BINOP_EVAL_H_

#include "AST.h"
#include "token.h"
#include "debug.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// a result could not be computed
// if variables or function calls
// were part of the calculation
typedef struct {
  bool computed;
  int value;
} BinopResult_t;

BinopResult_t *binop_evaluate(AST_t *node);
bool binop_iscomputable(AST_t *node);
int binop_evaluate_(AST_t *node);

#endif
