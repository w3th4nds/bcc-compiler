#ifndef _AST_H_
#define _AST_H_

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Each AST type uses different 
  AST_NUMS:
    - num_value
  AST_ID:
    - name
  AST_DECL:
    - specs_type
    - name
  AST_FUNCTION:
    - specs_type
    - name
    - children (=args - list of AST_DECLs)
    - body
  AST_CALL:
    - name
    - children (=params - list of AST_NUMs, AST_IDs, AST_CALLs)
  AST_ASSIGNMENT:
    - decl (if it includes one)
    - name (if it does not include a decl)
    - op
    - value (expression)
  AST_RETURN:
    - value (expression)
  AST_BINOP:
    - left
    - op
    - right (can be another AST_BINOP)
  AST_COMPOUND:
    - children (list of statements)
*/

typedef struct AST_STRUCT {
  // AST node type
  enum {
    AST_NUM,
    AST_ID,
    AST_DECL,
    AST_FUNCTION,
    AST_CALL,
    AST_ASSIGNMENT,
    AST_RETURN,
    AST_BINOP,
    AST_COMPOUND,
  } node_type;
  //--
  char *name;
  int specs_type;
  long num_value;
  int op;
  struct AST_STRUCT *value;
  struct AST_STRUCT *left;
  struct AST_STRUCT *right;
  struct AST_STRUCT *decl;
  struct AST_STRUCT *body;
  List_t *children;
} AST_t;

AST_t *init_ast(int node_type);
char *AST_type_to_str(int node_type);
char *AST_to_str(AST_t *ast);

#endif
