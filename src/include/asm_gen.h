#ifndef _ASM_GEN_H_
#define _ASM_GEN_H_

#include "debug.h"
#include "AST.h"
#include "asm_templates/return_template.h"
#include <stdlib.h>

char *asm_generate(AST_t *root);
char *asm_func_def(AST_t *node);
char *asm_return(AST_t *node);

#endif
