#ifndef _BCC_H_
#define _BCC_H_

#include "lexer.h"
#include "scope.h"
#include "parser.h"
#include "util.h"
#include "debug.h"
#include "asmgen_x86.h"
#include <stdlib.h>

char *bcc_read_file(char *fname);
void bcc_write_asm(char *outname, char *generated_asm);
void bcc_compile(char *src);
void bcc_compile_file(char *fname);

#endif
