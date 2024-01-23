#include "include/asm_gen.h"

char *asm_generate(AST_t *root)
{
  if (ASM_DEBUG) printf("asm_generate()\n");
  List_t *asm_list = init_list(sizeof(char *));
  printf("Root has %ld children\n", root->children->size);
  char *code;
  // TODO: push all code to a list, eventually
  for (int i = 0; i < root->children->size; ++i) {
    AST_t *node = root->children->items[i];
    switch (node->node_type) {
      case AST_FUNCTION:
        code = asm_func_def(root->children->items[i]);
        printf("\ngenerated:\n%s\n", code);
        break;
      default:
        printf("node_type = %s\n", AST_type_to_str(node->node_type));
        error_exit("asm_generate() - default reached\n");
    }
  }
  return code;
}

char *asm_func_def(AST_t *node)
{
  if (ASM_DEBUG) printf("asm_func_def()\n");
  AST_t *body = node->body;
  printf("func has %ld children\n", body->children->size);
  for (int i = 0; i < body->children->size; ++i) {
    AST_t *child = body->children->items[i];
    switch (child->node_type) {
      case AST_RETURN:
        char *code = asm_return(body->children->items[i]);
        return code;
      default:
        printf("node_type = %s\n", AST_type_to_str(node->node_type));
        error_exit("asm_func_def() - default reached\n");
    }
  }
}

char *asm_return(AST_t *node)
{
  if (ASM_DEBUG) printf("asm_return()\n");
  switch(node->value->node_type) {
    case AST_NUM:
      char *code = calloc(return_template_sz+128, sizeof(char));
      snprintf(code, return_template_sz+128, return_template_asm, node->value->num_value);
      return code;
    case AST_ID:
    case AST_BINOP:
    default:
      error_exit("asm_return - default case - implement\n");
  }
}

