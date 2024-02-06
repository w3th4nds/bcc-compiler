#include "include/macros.h"
#include "include/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer_t *init_lexer(char *src)
{
  Lexer_t *lexer = calloc(1, sizeof(Lexer_t));
  lexer->src = src;
  lexer->idx = 0;
  lexer->rollback_idx = 0;
  lexer->src_size = strlen(src);
  lexer->c = src[lexer->idx];
  if (LEX_DEBUG) {
    // get all generated tokens - no parsing
    lexer_test(lexer);
    exit(0);
  }
  return lexer;
}

void lexer_save(Lexer_t *lexer)
{
  lexer->rollback_idx = lexer->idx;
}

void lexer_rollback(Lexer_t *lexer)
{
  lexer->c = lexer->src[lexer->rollback_idx];
  lexer->idx = lexer->rollback_idx;
}

void lexer_advance(Lexer_t *lexer)
{
  if (lexer->idx < lexer->src_size && lexer->c != '\0') {
    lexer->idx++;
    lexer->c = lexer->src[lexer->idx];
  }
}

char lexer_peek(Lexer_t *lexer, int offset)
{
  return lexer->src[MIN(lexer->idx+offset, lexer->src_size)];
}

// Used for multi-character tokens, advances the lexer index
Token_t *lexer_advance_with(Lexer_t *lexer, Token_t *token)
{
  for (int i = 0; i < strlen(token->value); ++i) lexer_advance(lexer);
  return token;
}

// Used for single-character tokens
Token_t *lexer_advance_current(Lexer_t *lexer, TokenKind kind)
{
  char *value = calloc(2, sizeof(char));
  value[0] = lexer->c;
  Token_t *token = init_token(value, kind);
  lexer_advance(lexer);
  return token;
}

void lexer_skip_whitespace(Lexer_t *lexer)
{
  while (lexer->c == '\r' || lexer->c == '\n' || \
         lexer->c == ' ' || lexer->c == '\t') lexer_advance(lexer);
}
void lexer_skip_line(Lexer_t *lexer)
{
  do {
    lexer_advance(lexer);
    if (lexer->c == '\0') break;
  } while (lexer_peek(lexer, -1) != '\n');
}

void lexer_skip_to_comment_end(Lexer_t *lexer)
{
  do {
    lexer_advance(lexer);
    if (lexer->c == '\0') break;
  } while (lexer_peek(lexer, -1) != '/' || lexer_peek(lexer, -2) != '*');
}

Token_t *lexer_parse_id(Lexer_t *lexer)
{
  char *value = calloc(2, sizeof(char));
  value[0] = lexer->c;
  int off = 1;
  char lookahead = lexer_peek(lexer, off);;
  while (isalnum(lookahead) || lookahead == '_') {
    value = realloc(value, (strlen(value)+2) * sizeof(char));
    strcat(value, (char[]){lookahead, 0});
    lookahead = lexer_peek(lexer, ++off);
  }
  TokenKind token_kw;
  if ((token_kw = is_keyword(value)) == -1)
    return init_token(value, TOKEN_ID);
  else
    return init_token(value, token_kw);
}

Token_t *lexer_parse_num(Lexer_t *lexer)
{
  char *value = calloc(2, sizeof(char));
  value[0] = lexer->c;
  int off = 1;
  char lookahead = lexer_peek(lexer, off);
  while (isdigit(lookahead)) {
    value = realloc(value, (strlen(value)+2) * sizeof(char));
    strcat(value, (char[]){lookahead, 0});
    lookahead = lexer_peek(lexer, ++off);
  }
  return init_token(value, TOKEN_NUM);
}

Token_t *lexer_parse_hex(Lexer_t *lexer)
{
  char *value = calloc(3, sizeof(char));
  value[0] = lexer->c;
  value[1] = lexer_peek(lexer, 1);
  int off = 2;
  char lookahead = lexer_peek(lexer, off);
  while (isxdigit(lookahead)) {
    value = realloc(value, (strlen(value)+2) * sizeof(char));
    strcat(value, (char[]){lookahead, 0});
    lookahead = lexer_peek(lexer, ++off);
  }
  return init_token(value, TOKEN_NUM);
}

Token_t *lexer_parse_string(Lexer_t *lexer)
{
  lexer_advance(lexer); // skip the '"'
  char *value = calloc(2, sizeof(char));
  value[0] = lexer->c;
  int off = 1;
  char lookahead = lexer_peek(lexer, off);
  while (lookahead != '"') {
    value = realloc(value, (strlen(value)+2) * sizeof(char));
    strcat(value, (char[]){lookahead, 0});
    lookahead = lexer_peek(lexer, ++off);
  }
  lexer_advance(lexer); // skip the '"'
  return init_token(value, TOKEN_STRING);
}

Token_t *lexer_next_token(Lexer_t *lexer)
{
  while (lexer->c != '\0') {
    lexer_skip_whitespace(lexer);
    if (isalpha(lexer->c) || lexer->c == '_') return lexer_advance_with(lexer, lexer_parse_id(lexer));
    // special case for hex numbers 
    if (lexer->c == '0' && lexer_peek(lexer, 1) == 'x') return lexer_advance_with(lexer, lexer_parse_hex(lexer));
    if (isdigit(lexer->c)) return lexer_advance_with(lexer, lexer_parse_num(lexer));
    char lookahead = lexer_peek(lexer, 1);
    switch (lexer->c) {
      case '\0': return init_token(0, TOKEN_EOF);;
      case '(': return lexer_advance_current(lexer, TOKEN_LP);
      case ')': return lexer_advance_current(lexer, TOKEN_RP);
      case '{': return lexer_advance_current(lexer, TOKEN_LBRACE);
      case '}': return lexer_advance_current(lexer, TOKEN_RBRACE);
      case ':': return lexer_advance_current(lexer, TOKEN_COLON);
      case ',': return lexer_advance_current(lexer, TOKEN_COMMA);
      case ';': return lexer_advance_current(lexer, TOKEN_SEMI);
      case '[': return lexer_advance_current(lexer, TOKEN_LBRACKET);
      case ']': return lexer_advance_current(lexer, TOKEN_RBRACKET);
      case '"': return lexer_advance_with(lexer, lexer_parse_string(lexer));
      case '=':
        if (lexer_peek(lexer, 1) == '=') return lexer_advance_with(lexer, init_token("==", TOKEN_EQUALSEQUALS));
        else return lexer_advance_current(lexer, TOKEN_EQUALS);
      case '+':
        if (lookahead == '+') return lexer_advance_with(lexer, init_token("++", TOKEN_PLUSPLUS));
        else if (lookahead == '=') return lexer_advance_with(lexer, init_token("+=", TOKEN_PLUSEQUALS));
        else return lexer_advance_current(lexer, TOKEN_PLUS);
      case '-':
        if (lookahead == '-') return lexer_advance_with(lexer, init_token("--", TOKEN_MINUSMINUS));
        else if (lookahead == '=') return lexer_advance_with(lexer, init_token("-=", TOKEN_MINUSEQUALS));
        else return lexer_advance_current(lexer, TOKEN_MINUS);
      case '!':
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("!=", TOKEN_NOTEQUALS));
        else return lexer_advance_current(lexer, TOKEN_NOT);
      case '^':
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("^=", TOKEN_XOREQUALS));
        else return lexer_advance_current(lexer, TOKEN_XOR);
      case '/':
        if (lookahead == '/') lexer_skip_line(lexer);
        else if (lookahead == '*') lexer_skip_to_comment_end(lexer);
        else if (lookahead == '=') return lexer_advance_with(lexer, init_token("/=", TOKEN_DIVEQUALS));
        else return lexer_advance_current(lexer, TOKEN_DIV);
        break;
      case '*': // how to handle pointers ?
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("*=", TOKEN_MULEQUALS));
        else return lexer_advance_current(lexer, TOKEN_MUL);
      case '%':
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("*=", TOKEN_MODEQUALS));
        else return lexer_advance_current(lexer, TOKEN_MOD);
      case '<':
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("<=", TOKEN_LE));
        else if (lookahead == '<') return lexer_advance_with(lexer, init_token("<<", TOKEN_LBITSHIFT));
        else return lexer_advance_current(lexer, TOKEN_LT);
      case '>':
        if (lookahead == '=') return lexer_advance_with(lexer, init_token("<=", TOKEN_GE));
        else if (lookahead == '>') return lexer_advance_with(lexer, init_token("<<", TOKEN_RBITSHIFT));
        else return lexer_advance_current(lexer, TOKEN_GT);
      default:
        printf("Implement: %c (%d)\n", lexer->c, lexer->c);
        error_exit("non implemented case for lexer");
    }
  }
  return init_token(0, TOKEN_EOF);
}

TokenKind is_keyword(char *value)
{
  // expand as necessary
  if (strcmp(value, KW_RETURN) == 0)  return TOKEN_RETURN;
  if (strcmp(value, KW_INT) == 0)     return TOKEN_INT;
  if (strcmp(value, KW_LONG) == 0)    return TOKEN_LONG;
  if (strcmp(value, KW_VOID) == 0)    return TOKEN_VOID;
  else return -1;
}

void lexer_test(Lexer_t *lexer)
{
  Token_t *token = NULL;
  while ((token = lexer_next_token(lexer))->kind != TOKEN_EOF)
    print_token(token);
}
