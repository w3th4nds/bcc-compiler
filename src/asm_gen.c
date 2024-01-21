#include "include/asm_gen.h"

int asm_return(AST_t *node)
{
  if (node->ast_type != AST_RETURN) error_exit("asm_return() - node ast type != AST_RETURN\n");
  switch(node->value->ast_type) {
    case AST_NUM:
      break;
    case AST_ID:
    case AST_BINOP:
    default:
      error_exit("asm_return - default case - implement\n");
  }
  return template;
}
