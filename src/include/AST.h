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
  AST_NOP: -> empty
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
  AST_FOR:
    - stmt1
    - stmt2
    - stmt3
    - body
*/

// AST types
typedef enum {
  AST_NOP,
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
  AST_FOR,
} AstType_t;

typedef struct AST_STRUCT {
  AstType_t node_type;
  char *name;
  Type_t specs_type;
  long num_value;
  TokenKind_t op;
  union {
    struct AST_STRUCT *value;
    struct AST_STRUCT *body;
  };
  struct AST_STRUCT *left;
  struct AST_STRUCT *right;
  struct AST_STRUCT *decl;
  struct AST_STRUCT *cond;
  struct AST_STRUCT *stmt1;
  struct AST_STRUCT *stmt2;
  struct AST_STRUCT *stmt3;
  // different names for clarity
  union {
    List_t *children;
    List_t *params;
    List_t *args;
  };
  // only used for creating graphs
  long node_id;
} AST_t;

AST_t *init_ast(AstType_t node_type);
char *AST_type_to_str(AstType_t node_type);

#endif
