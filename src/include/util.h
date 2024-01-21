#ifndef _UTIL_H_
#define _UTIL_H_

#include "list.h"
#include "AST.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

char *format(AST_t *node);
void create_ast_file(AST_t *root);

#endif
