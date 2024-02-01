#ifndef _LEXER_H_
#define _LEXER_H_

#include "debug.h"
#include "token.h"
#include "keywords.h"
#include <stdio.h>

typedef struct  {
  char c;
  unsigned int idx;
  unsigned int rollback_idx;
  char *src;
  size_t src_size;
} Lexer_t;

Lexer_t *init_lexer(char *src);
void lexer_save(Lexer_t *lexer);
void lexer_rollback(Lexer_t *lexer);
void lexer_advance(Lexer_t *lexer);
char lexer_peek(Lexer_t *lexer, int offset);
Token_t *lexer_next_token(Lexer_t *lexer);
void lexer_skip_whitespace(Lexer_t *lexer);
void lexer_skip_line(Lexer_t *lexer);
void lexer_skip_to_comment_end(Lexer_t *lexer);
Token_t *lexer_parse_id(Lexer_t *lexer);
Token_t *lexer_parse_num(Lexer_t *lexer);
Token_t *lexer_parse_hex(Lexer_t *lexer);
Token_t *lexer_next_token(Lexer_t *lexer);
Token_t *lexer_advance_with(Lexer_t *lexer, Token_t *token);
Token_t *lexer_advance_current(Lexer_t *lexer, int kind);
int is_keyword(char *value);
void lexer_test(Lexer_t *lexer);

#endif
