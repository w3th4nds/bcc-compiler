#include "include/types.h"

int match_type(int token_kind)
{
  switch (token_kind) {
    case TOKEN_VOID:  return TYPE_VOID;
    case TOKEN_INT:   return TYPE_INT;
    case TOKEN_LONG:  return TYPE_LONG;
    default:          return NO_TYPE;
  }
}

char *type_to_str(int type)
{
  switch (type) {
    case NO_TYPE:     return "NO_TYPE";
    case TYPE_VOID:   return "TYPE_VOID";
    case TYPE_INT:    return "TYPE_INT";
    case TYPE_LONG:   return "TYPE_LONG";
    default:          return "type_to_str() - implement";
  }
}

size_t type_size(int type)
{
  switch (type) {
    case TYPE_VOID:   return sizeof(void);
    case TYPE_INT:    return sizeof(int);
    case TYPE_LONG:   return sizeof(long);
    case NO_TYPE:
    default:          error_exit("type_size - request for NO_TYPE / non implemented type\n");
  }
  return 0;
}

// does not fully belong in types, but w/e
bool is_assign_op(int token_kind)
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
