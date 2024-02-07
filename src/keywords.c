#include "include/keywords.h"

TokenKind_t is_keyword(char *value)
{
  // expand as necessary
  if (strcmp(value, KW_RETURN) == 0)  return TOKEN_RETURN;
  if (strcmp(value, KW_INT) == 0)     return TOKEN_INT;
  if (strcmp(value, KW_LONG) == 0)    return TOKEN_LONG;
  if (strcmp(value, KW_VOID) == 0)    return TOKEN_VOID;
  if (strcmp(value, KW_WHILE) == 0)   return TOKEN_WHILE;
  else return -1;
}