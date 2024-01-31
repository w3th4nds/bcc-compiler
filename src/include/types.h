#ifndef _TYPE_H_
#define _TYPE_H_

#include "debug.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum {
  NO_TYPE = -1,
  TYPE_VOID,
  TYPE_INT,
  TYPE_LONG,
};

int match_type(int token_kind);
char *type_to_str(int type);
size_t type_size(int type);
bool is_assign_op(int token_kind);
bool is_binop_op(int token_kind);

#endif
