#include "include/asm_gen.h"

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root)
{
  if (ASM_DEBUG) printf("asm_generate()\n");
  List_t *asm_list = init_list(sizeof(char *));
  char *code = calloc(2, sizeof(char));

  // declare functions as global
  List_t *ids = hashmap_get_all_ids(scope_manager->scopes);
  int i;
  size_t newsz;
  for (i = 0; i < ids->size; ++i) {
    newsz = strlen(code) + strlen("global ") + strlen(ids->items[i]) + 2; // 2 = 1 null byte + 1 LF
    code = realloc(code, newsz);
    snprintf(code+strlen(code), newsz, "global %s\n", (char *)ids->items[i]);
  }

  // .text section
  char *textsect = "\nsection .text\n\n";
  newsz = strlen(code) + strlen(textsect) + 1;
  code = realloc(code, newsz);
  strcat(code, textsect);

  // TODO: push all code to a list, eventually
  char *next_code;
  for (i = 0; i < root->children->size; ++i) {
    AST_t *node = root->children->items[i];
    switch (node->node_type) {
      case AST_FUNCTION:
        next_code = asm_func_def(scope_manager, root->children->items[i]);
        break;
      default:
        printf("node_type = %s\n", AST_type_to_str(node->node_type));
        error_exit("asm_generate() - default reached\n");
    }
    code = realloc(code, strlen(code)+strlen(next_code)+1);
    strcat(code, next_code);
  }
  return code;
}

char *asm_func_def(ScopeManager_t *scope_manager, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_func_def()\n");
  char *code = calloc(funcdef_template_sz+128, sizeof(char));
  Bucket_t *bucket = hashmap_getitem(scope_manager->scopes, node->name);
  snprintf(code, funcdef_template_sz+128, funcdef_template_asm, node->name);
  // set the scope to make it easier
  scope_set(scope_manager, node->name);

  // account for 128-byte stack red-zone
  // allocate stack space as necessary
  if (bucket->symtab->offset > 128) {
    char *subrsp = calloc(32, sizeof(char));
    snprintf(subrsp, 32, "sub rsp, %d\n", bucket->symtab->offset - 128);
  }

  char *next_code;
  List_t *list = node->body->children;
  for (int i = 0; i < list->size; ++i) {
    AST_t *child = list->items[i];
    switch (child->node_type) {
      case AST_RETURN:
        next_code = asm_return(scope_manager, list->items[i]);
        break;
      case AST_ASSIGNMENT:
        next_code = asm_assignment(scope_manager, list->items[i]);
        break;
      case AST_DECL:
      default:
        printf("child node_type = %s\n", AST_type_to_str(child->node_type));
        error_exit("asm_func_def() - default reached\n");
    }
    code = realloc(code, strlen(code) + strlen(next_code) + 1);
    strcat(code, next_code);
  }
  return code;
}

char *asm_return(ScopeManager_t *scope_manager, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_return()\n");
  char *code;
  switch(node->value->node_type) {
    case AST_NUM:
      code = calloc(return_template_sz+128, sizeof(char));
      snprintf(code, return_template_sz+128, return_template_asm, node->value->num_value);
      break;
    case AST_BINOP:
      // in case the binop can be calculated on the spot
      BinopResult_t *res = binop_evaluate(node->value);
      if (res->computed) {
        code = calloc(return_template_sz+128, sizeof(char));
        snprintf(code, return_template_sz+128, return_template_asm, res->value);
      }
      else {
        error_exit("asm_return() - binop could not be calculated\n");
      }
      break;
    case AST_ID:
    case AST_CALL:
    default:
      error_exit("asm_return - default case - implement\n");
  }
  return code;
}

char *asm_assignment(ScopeManager_t *scope_manager, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_assignment\n");
  error_exit("implement asm_assignment()\n");
  return NULL;
}
