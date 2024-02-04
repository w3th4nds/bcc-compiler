#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int SHOW_SOURCE;
extern int PLOT_EXPR;
extern int LEX_DEBUG;
extern int PARSE_DEBUG;
extern int SCOPE_DEBUG;
extern int ASM_DEBUG;
void init_debug(void);
void error_exit(char *msg);

#endif
