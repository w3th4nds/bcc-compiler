#include "include/bcc.h"
#include "include/lexer.h"
#include "include/scope.h"
#include "include/parser.h"
#include "include/util.h"
#include "include/debug.h"
#include "include/asm_gen.h"
#include <stdlib.h>

char *bcc_read_file(char *fname)
{
  FILE *file;
  if ((file = fopen(fname, "r")) == NULL)
    error_exit("Error opening input file.\n");
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  //printf("Source file size = %ld bytes\n", file_size);
  char *src = calloc(1, file_size+1);
  fread(src, sizeof(char), file_size+1, file);
  fclose(file);
  return src;
}

void bcc_write_asm(char *outname, char *generated_asm)
{
  FILE *file;
  if ((file = fopen(outname, "w")) == NULL)
    error_exit("Error opening output file.\n");
  fwrite(generated_asm, sizeof(char), strlen(generated_asm), file);
}

void bcc_compile(char *src)
{
  // inits
  init_debug();
  if (SHOW_SOURCE) printf("Source Code:\n%s\n", src);
  Lexer_t *lexer = init_lexer(src);
  ScopeManager_t *scope_manager = init_scope_manager();
  Parser_t *parser = init_parser(lexer, scope_manager);
  // parse AST
  AST_t *root = parser_parse(parser);
  if (SCOPE_DEBUG) print_scopes(scope_manager);
  char *generated_assembly = asm_generate(scope_manager, root);
  if (ASM_DEBUG) printf("\nGenerated ASM =\n%s", generated_assembly);
  bcc_write_asm("out.s", generated_assembly);
  printf("[ SUCCESS ]\n");
}

void bcc_compile_file(char *fname)
{
  char *src = bcc_read_file(fname);
  bcc_compile(src);
  free(src);
}
