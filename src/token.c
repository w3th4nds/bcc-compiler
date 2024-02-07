#include "include/token.h"
#include <stdlib.h>
#include <stdio.h>

Token_t *init_token(char *value, TokenKind kind)
{
  Token_t *token = calloc(1, sizeof(Token_t));
  token->value = value;
  token->kind = kind;
  return token;
}

const char *token_kind_to_str(TokenKind kind)
{
  switch (kind) {
    case TOKEN_ID:            return "TOKEN_ID";
    case TOKEN_NUM:           return "TOKEN_NUM";
    case TOKEN_EQUALS:        return "TOKEN_EQUALS";
    case TOKEN_EQUALSEQUALS:  return "TOKEN_EQUALSEQUALS";
    case TOKEN_NOT:           return "TOKEN_NOT";
    case TOKEN_NOTEQUALS:     return "TOKEN_NOTEQUALS";
    case TOKEN_GE:            return "TOKEN_GE";
    case TOKEN_LE:            return "TOKEN_LE";
    case TOKEN_SEMI:          return "TOKEN_SEMI";
    case TOKEN_PLUS:          return "TOKEN_PLUS";
    case TOKEN_PLUSPLUS:      return "TOKEN_PLUSPLUS";
    case TOKEN_PLUSEQUALS:    return "TOKEN_PLUSEQUALS";
    case TOKEN_MINUS:         return "TOKEN_MINUS";
    case TOKEN_MINUSMINUS:    return "TOKEN_MINUSMINUS";
    case TOKEN_MINUSEQUALS:   return "TOKEN_MINUSEQUALS";
    case TOKEN_DIV:           return "TOKEN_DIV";
    case TOKEN_DIVEQUALS:     return "TOKEN_DIVEQUALS";
    case TOKEN_MUL:           return "TOKEN_MUL";
    case TOKEN_MULEQUALS:     return "TOKEN_MULEQUALS";
    case TOKEN_XOR:           return "TOKEN_XOR";
    case TOKEN_XOREQUALS:     return "TOKEN_XOREQUALS";
    case TOKEN_MOD:           return "TOKEN_MOD";
    case TOKEN_MODEQUALS:     return "TOKEN_MODEQUALS";
    case TOKEN_LP:            return "TOKEN_LP";
    case TOKEN_RP:            return "TOKEN_RP";
    case TOKEN_LARROW:        return "TOKEN_LARROW";
    case TOKEN_RARROW:        return "TOKEN_RARROW";
    case TOKEN_LBRACE:        return "TOKEN_LBRACE";
    case TOKEN_RBRACE:        return "TOKEN_RBRACE";
    case TOKEN_LT:            return "TOKEN_LT";
    case TOKEN_GT:            return "TOKEN_GT";
    case TOKEN_LBITSHIFT:     return "TOKEN_LBITSHIFT";
    case TOKEN_RBITSHIFT:     return "TOKEN_RBITSHIFT";
    case TOKEN_STRING:        return "TOKEN_STRING";
    case TOKEN_COMMA:         return "TOKEN_COMMA";
    case TOKEN_COLON:         return "TOKEN_COLON";
    case TOKEN_LBRACKET:      return "TOKEN_LBRACKET";
    case TOKEN_RBRACKET:      return "TOKEN_RBRACKET";
    case TOKEN_EOF:           return "TOKEN_EOF";
    case TOKEN_RETURN:        return "TOKEN_RETURN";
    case TOKEN_INT:           return "TOKEN_INT";
    case TOKEN_LONG:          return "TOKEN_LONG";
    case TOKEN_VOID:          return "TOKEN_VOID";
    case TOKEN_WHILE:         return "TOKEN_WHILE";
    default:                  return "**IMPLEMENT**";
  }
}

void print_token(Token_t *token)
{
  printf("<Token=`%s`, Kind=`%s`>\n", token->value, token_kind_to_str(token->kind));
}

bool is_assign_op(TokenKind token_kind)
{
  if (token_kind == TOKEN_EQUALS || \
      token_kind == TOKEN_PLUSEQUALS || \
      token_kind == TOKEN_MINUSEQUALS || \
      token_kind == TOKEN_DIVEQUALS || \
      token_kind == TOKEN_MULEQUALS || \
      token_kind == TOKEN_XOREQUALS || \
      token_kind == TOKEN_MODEQUALS) return true;
  return false;
}

bool is_binop_op(TokenKind token_kind)
{
  if (token_kind == TOKEN_PLUS || \
      token_kind == TOKEN_MINUS || \
      token_kind == TOKEN_MUL || \
      token_kind == TOKEN_DIV) return true;
  return false;
}

bool is_cond_op(TokenKind token_kind)
{
  if (token_kind == TOKEN_GT || \
      token_kind == TOKEN_GE || \
      token_kind == TOKEN_LT || \
      token_kind == TOKEN_LE || \
      token_kind == TOKEN_EQUALSEQUALS) return true;
  return false;
}

char *op_to_str(TokenKind op)
{
  switch (op) {
    case TOKEN_EQUALS:        return "=";
    case TOKEN_EQUALSEQUALS:  return "==";
    case TOKEN_PLUS:          return "+";
    case TOKEN_MINUS:         return "-";
    case TOKEN_MUL:           return "*";
    case TOKEN_DIV:           return "/";
    case TOKEN_GT:            return ">";
    case TOKEN_GE:            return ">=";
    case TOKEN_LT:            return "<";
    case TOKEN_LE:            return "<=";
    default:
      printf("op = %d\n", op);
      error_exit("op_to_str() - unknown op\n");
  }
  return NULL;
}