#ifndef _TYPE_H_
#define _TYPE_H_

#include "debug.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  NO_TYPE = -1,
  TYPE_VOID,
  TYPE_INT,
  TYPE_LONG,
} Type;

Type match_type(TokenKind token_kind);
char *type_to_str(Type type);
size_t type_size(Type type);

#endif
