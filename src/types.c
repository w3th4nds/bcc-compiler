#include "include/types.h"

Type_t match_type(TokenKind_t token_kind)
{
  switch (token_kind) {
    case TOKEN_VOID:  return TYPE_VOID;
    case TOKEN_INT:   return TYPE_INT;
    case TOKEN_LONG:  return TYPE_LONG;
    default:          return NO_TYPE;
  }
}

char *type_to_str(Type_t type)
{
  switch (type) {
    case NO_TYPE:   return "NO_TYPE";
    case TYPE_VOID: return "TYPE_VOID";
    case TYPE_INT:  return "TYPE_INT";
    case TYPE_LONG: return "TYPE_LONG";
    default: error_exit("type_to_str() - implement\n");
  }
  return NULL;
}

size_t type_size(Type_t type)
{
  switch (type) {
    case TYPE_VOID: return sizeof(void);
    case TYPE_INT:  return sizeof(int);
    case TYPE_LONG: return sizeof(long);
    case NO_TYPE:
    default: error_exit("type_size - request for NO_TYPE / non implemented type\n");
  }
  return NO_TYPE;
}
