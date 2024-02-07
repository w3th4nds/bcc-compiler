#ifndef _AST_H_
#define _AST_H_

#include "list.h"
#include "token.h"
#include "types.h"
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
    - params
    - body
  AST_CALL:
    - name
    - args
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
  AST_COND: - same as binop but w/ different type name
    - left
    - op
    - right
  AST_WHILE:
    - cond
    - body
*/

// AST types
typedef enum {
  AST_NUM,
  AST_ID,
  AST_DECL,
  AST_FUNCTION,
  AST_CALL,
  AST_ASSIGNMENT,
  AST_RETURN,
  AST_BINOP,
  AST_COMPOUND,
  AST_COND,
  AST_WHILE,
} AstType;

typedef struct AST_STRUCT {
  AstType node_type;
  char *name;
  Type specs_type;
  long num_value;
  TokenKind op;
  union {
    struct AST_STRUCT *value;
    struct AST_STRUCT *body;
  };
  struct AST_STRUCT *left;
  struct AST_STRUCT *right;
  struct AST_STRUCT *decl;
  struct AST_STRUCT *cond;
  // different names for clarity
  union {
    List_t *children;
    List_t *params;
    List_t *args;
  };
  // only used for creating graphs
  long node_id;
} AST_t;

AST_t *init_ast(AstType node_type);
char *AST_type_to_str(AstType node_type);
char *AST_to_str(AST_t *ast);

#endif
