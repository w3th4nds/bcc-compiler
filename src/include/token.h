#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "debug.h"
#include <stdbool.h>

typedef enum {
  TOKEN_ID = 0,
  TOKEN_NUM,
  TOKEN_EQUALS,
  TOKEN_EQEQ,
  TOKEN_NOT,
  TOKEN_NOTEQ,
  TOKEN_GE,
  TOKEN_LE,
  TOKEN_SEMI,
  TOKEN_PLUS,
  TOKEN_PLUSPLUS,
  TOKEN_PLUSEQ,
  TOKEN_MINUS,
  TOKEN_MINUSMINUS,
  TOKEN_MINUSEQ,
  TOKEN_DIV,
  TOKEN_DIVEQ,
  TOKEN_MUL,
  TOKEN_MULEQ,
  TOKEN_XOR,
  TOKEN_XOREQ,
  TOKEN_MOD,
  TOKEN_MODEQ,
  TOKEN_LP,
  TOKEN_RP,
  TOKEN_LARROW,
  TOKEN_RARROW,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LBITSHIFT,
  TOKEN_RBITSHIFT,
  TOKEN_STRING,
  TOKEN_COMMA,
  TOKEN_COLON,
  // keyword-specific
  TOKEN_RETURN,
  TOKEN_INT,
  TOKEN_LONG,
  TOKEN_VOID,
  TOKEN_WHILE,
  TOKEN_EOF
} TokenKind_t;

typedef struct {
  char *value;
  TokenKind_t kind;
} Token_t;

Token_t *init_token(char *value, TokenKind_t kind);
const char *token_kind_to_str(TokenKind_t kind);
void print_token(Token_t *token);
bool is_assign_op(TokenKind_t token_kind);
bool is_binop_op(TokenKind_t token_kind);
bool is_cond_op(TokenKind_t token_kind);
char *op_to_str(TokenKind_t op);

#endif
