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
    //printf("code so far = \n%s\n", code);
  }
  return code;
}

char *asm_func_def(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_func_def()\n");
  char *code = calloc(funcdef_template_sz+128, sizeof(char));
  Bucket_t *bucket = hashmap_getbucket(ctx->scope_manager->scopes, ctx->node->name);
  snprintf(code, funcdef_template_sz+128, funcdef_template_asm, ctx->node->name);

  // TODO: make params work

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
  size_t sz;
  char *code, *template;
  switch(ctx->node->value->node_type) {
    case AST_NUM:
      sz = strlen(return_template_num) + 36;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, return_template_num, ctx->node->value->num_value);
      break;
    case AST_BINOP:
      BinopResult_t *res = binop_evaluate(ctx->node->value);
      // if it can be computed on the spot, do that
      if (res->computed) {
        sz = strlen(return_template_num) + 36;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, return_template_num, res->value);
      }
      else {
        // TODO: implement
        error_exit("asm_return() - binop could not be calculated\n");
      }
      break;
    case AST_ID:
      SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, ctx->node->value->name);
      switch (entry->size) {
        case 1: template = return_template_var1; break;
        case 2: template = return_template_var2; break;
        case 4: template = return_template_var4; break;
        case 8: template = return_template_var8; break;
        default: error_exit("asm_return() - case AST_ID: size check default reached\n");
      }
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, entry->offset);
      break;
    case AST_CALL:
      // TODO: make this prettier
      AST_t *tmp = ctx->node;
      ctx->node = ctx->node->value;
      code = asm_call(ctx);
      ctx->node = tmp;
      template = "pop rbp\nret\n\n";
      sz = strlen(template) + 1;
      code = realloc(code, strlen(code) + sz);
      strcpy(code+strlen(code), template);
      break;
    default:
      error_exit("asm_return - default case - implement\n");
  }
  return code;
}

char *asm_assignment(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_assignment()\n");
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
  
  char *assign_template;
  switch (entry->size) {
    case 1: assign_template = "mov byte [rbp-0x%x], 0x%x\n";  break;
    case 2: assign_template = "mov word [rbp-0x%x], 0x%x\n";  break;
    case 4: assign_template = "mov dword [rbp-0x%x], 0x%lx\n"; break;
    case 8: assign_template = "mov qword [rbp-0x%x], 0x%llx\n"; break;
    default: error_exit("asm_assignment() - size check default reached\n");
  }

  size_t assign_template_sz = strlen(assign_template) + 32;
  char *code = calloc(assign_template_sz, sizeof(char));
  snprintf(code, assign_template_sz, assign_template, entry->offset, value);

  return code;
}

char *asm_call(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_call()\n");
  char *code = calloc(1, sizeof(char));
  // TODO: add support for func calls as args
  char *template_num = "mov %s, 0x%x\n";
  char *template_id;
  size_t sz;
  // put params in registers
  List_t *params = ctx->node->children;
  for (int i = 0; i < params->size; ++i) {
    AST_t *param = params->items[i];
    char *reg = get_param_register(i);
    switch (param->node_type) {
      case AST_NUM:
        // use strcat here
        sz = strlen(template_num) + 24;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template_num, reg, param->num_value);
        break;
      case AST_ID:
        // TODO: fix repeating code, make more defined templates
        // ugly embedded switch stmt
        SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, param->name);
        switch (entry->size) {
          case 1: template_id = "mov %s, byte [rbp-0x%x]\n";  break;
          case 2: template_id = "mov %s, word [rbp-0x%x]\n";  break;
          case 4: template_id = "mov %s, dword [rbp-0x%x]\n"; break;
          case 8: template_id = "mov %s, qword [rbp-0x%x]\n"; break;
          default: error_exit("asm_call() - param size check default reached\n");
        }
        sz = strlen(code) + strlen(template_id) + 36;
        code = realloc(code, sz);
        snprintf(code, sz, template_id, reg, entry->offset);
        break;
      default:      error_exit("asm_call - param default case reached\n");
    }
  }

  char *call_template = "call %s\n";
  sz = strlen(call_template) + 36;
  code = realloc(code, strlen(code) + sz);
  snprintf(code+strlen(code), sz, call_template, ctx->node->name);

  return code;
}