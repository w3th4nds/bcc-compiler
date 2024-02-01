#include "include/asm_gen.h"

AsmCtx_t *init_asm_ctx(ScopeManager_t *scope_manager, AST_t *node)
{
  AsmCtx_t *ctx = calloc(1, sizeof(AsmCtx_t));
  ctx->scope_manager = scope_manager;
  ctx->node = node;
  return ctx;
}

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root)
{
  if (ASM_DEBUG) printf("asm_generate()\n");
  AsmCtx_t *ctx = init_asm_ctx(scope_manager, root);

  // maybe this will be used eventually
  List_t *asm_list = init_list(sizeof(char *));

  char *code = calloc(2, sizeof(char));
  // declare functions as global
  List_t *ids = hashmap_get_all_ids(scope_manager->scopes);
  for (int i = 0; i < ids->size; ++i) {
    size_t newsz = strlen(code) + strlen("global \n") + strlen(ids->items[i]) + 1;
    code = realloc(code, newsz);
    snprintf(code+strlen(code), newsz, "global %s\n", (char *)ids->items[i]);
  }

  // .text section
  char *textsect = "\nsection .text\n\n";
  size_t newsz = strlen(code) + strlen(textsect) + 1;
  code = realloc(code, newsz);
  strcat(code, textsect);

  char *next_code;
  for (int i = 0; i < root->children->size; ++i) {
    ctx->node = root->children->items[i];
    switch (ctx->node->node_type) {
      case AST_FUNCTION:
        next_code = asm_func_def(ctx);
        break;
      default:
        printf("node_type = %s\n", AST_type_to_str(ctx->node->node_type));
        error_exit("asm_generate() - default reached\n");
    }
    code = realloc(code, strlen(code) + strlen(next_code) + 1);
    strcat(code, next_code);
  }
  return code;
}

char *asm_func_def(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_func_def()\n");
  char *code = calloc(funcdef_template_sz+128, sizeof(char));
  Bucket_t *bucket = hashmap_getbucket(ctx->scope_manager->scopes, ctx->node->name);
  snprintf(code, funcdef_template_sz+128, funcdef_template_asm, ctx->node->name);

  // set the scope to make things easier
  scope_set(ctx->scope_manager, ctx->node->name);

  // account for 128-byte stack red-zone (see notes)
  // allocate stack space as necessary
  if (bucket->symtab->offset > 128) {
    char *subrsp = calloc(32, sizeof(char));
    snprintf(subrsp, 32, "sub rsp, %d\n", bucket->symtab->offset - 128);
  }

  char *next_code;
  List_t *list = ctx->node->body->children;
  for (int i = 0; i < list->size; ++i) {
    ctx->node = list->items[i];
    switch (ctx->node->node_type) {
      case AST_RETURN:
        next_code = asm_return(ctx);
        break;
      case AST_ASSIGNMENT:
        next_code = asm_assignment(ctx);
        break;
      case AST_DECL:
      default:
        printf("child node_type = %s\n", AST_type_to_str(ctx->node->node_type));
        error_exit("asm_func_def() - default reached\n");
    }
    code = realloc(code, strlen(code) + strlen(next_code) + 1);
    strcat(code, next_code);
  }
  return code;
}

char *asm_return(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_return()\n");
  char *code;
  switch(ctx->node->value->node_type) {
    case AST_NUM:
      code = calloc(return_template_sz+128, sizeof(char));
      snprintf(code, return_template_sz+128, return_template_asm, ctx->node->value->num_value);
      break;
    case AST_BINOP:
      // in case the binop can be calculated on the spot
      BinopResult_t *res = binop_evaluate(ctx->node->value);
      if (res->computed) {
        code = calloc(return_template_sz+128, sizeof(char));
        snprintf(code, return_template_sz+128, return_template_asm, res->value);
      }
      else {
        // TODO: implement
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

char *asm_assignment(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_assignment\n");
  char *id = ctx->node->decl == NULL ? ctx->node->name : ctx->node->decl->name;
  SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, id);

  // TODO: revisit and fix
  // this structure does not work with calls
  // or expressions that cannot be computed on the spot
  int value;
  switch (ctx->node->value->node_type) {
    case AST_NUM:
      value = ctx->node->value->num_value;
      break;
    case AST_BINOP:
    default:
      error_exit("asm_assignment() - default reached\n");
  }
  
  char *assign_template = "mov dword [rbp-%d], %d\n";
  size_t assign_template_sz = strlen(assign_template) + 32;
  char *code = calloc(assign_template_sz, sizeof(char));
  snprintf(code, assign_template_sz, assign_template, entry->offset, value);

  return code;
}
