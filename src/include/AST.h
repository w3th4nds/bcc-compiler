#ifndef _AST_H_
#define _AST_H_

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AST_STRUCT {
  // compound statements - decl args
  List_t *children;

  // assignments - func names
  char *name;
  struct AST_STRUCT *value;
  int num_value;
  int specs_type;

  // binops
  struct AST_STRUCT *left;
  struct AST_STRUCT *right;
  int op;

  // functions
  struct AST_STRUCT *decl;
  struct AST_STRUCT *body;

  // AST node type
  enum {
    AST_NUM,
    AST_ID,
    AST_SPECS,
    AST_DECL,
    AST_FUNCTION,
    AST_CALL,
    AST_ASSIGNMENT,
    AST_RETURN,
    AST_BINOP,
    AST_COMPOUND,
  } node_type;
} AST_t;

AST_t *init_ast(int node_type);
char *AST_type_to_str(int node_type);
char *AST_to_str(AST_t *ast);

#endif
