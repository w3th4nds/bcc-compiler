#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
  TOKEN_ID = 0,
  TOKEN_NUM,
  TOKEN_EQUALS,
  TOKEN_EQUALSEQUALS,
  TOKEN_NOT,
  TOKEN_NOTEQUALS,
  TOKEN_GE,
  TOKEN_LE,
  TOKEN_SEMI,
  TOKEN_PLUS,
  TOKEN_PLUSPLUS,
  TOKEN_PLUSEQUALS,
  TOKEN_MINUS,
  TOKEN_MINUSMINUS,
  TOKEN_MINUSEQUALS,
  TOKEN_DIV,
  TOKEN_DIVEQUALS,
  TOKEN_MUL,
  TOKEN_MULEQUALS,
  TOKEN_XOR,
  TOKEN_XOREQUALS,
  TOKEN_MOD,
  TOKEN_MODEQUALS,
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
  TOKEN_EOF
} TokenKind;

typedef struct {
  char *value;
  TokenKind kind;
} Token_t;

Token_t *init_token(char *value, TokenKind kind);
const char *token_kind_to_str(TokenKind kind);
void print_token(Token_t *token);

#endif
