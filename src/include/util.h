#ifndef _UTIL_H_
#define _UTIL_H_

#include "list.h"
#include "AST.h"
#include "token.h"
#include "debug.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

char *format(AST_t *node);
void make_ast_graph(AST_t *expr);

#endif
