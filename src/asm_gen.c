#include "include/asm_gen.h"

AsmCtx_t *init_asm_ctx(ScopeManager_t *scope_manager, AST_t *node)
{
  AsmCtx_t *ctx = calloc(1, sizeof(AsmCtx_t));
  ctx->scope_manager = scope_manager;
  ctx->reg_manager = init_register_manager();
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
  // set the scope to make things easier
  scope_set(ctx->scope_manager, ctx->node->name);
  Scope_t *scope = hashmap_getscope(ctx->scope_manager->scopes, ctx->node->name);
  char *template = "%s:\npush rbp\nmov rbp, rsp\n";
  size_t sz = strlen(template) + 64;
  char *code = calloc(sz, sizeof(char));
  snprintf(code, sz, template, scope->scope_id);

  // TODO: implement params
  // search no only in the current scope's symtab,
  // but in params too
  // eventually even global scope
  // ...

  char *func_code = calloc(1, sizeof(char));
  char *next_code;
  List_t *list = ctx->node->body->children;
  for (int i = 0; i < list->size; ++i) {
    ctx->node = list->items[i];
    switch (ctx->node->node_type) {
      case AST_RETURN:      next_code = asm_return(ctx);      break;
      case AST_ASSIGNMENT:  next_code = asm_assignment(ctx);  break;
      case AST_DECL: break; // does nothing in asm phase
      default:
        printf("child node_type = %s\n", AST_type_to_str(ctx->node->node_type));
        error_exit("asm_func_def() - default reached\n");
    }
    func_code = realloc(func_code, strlen(func_code) + strlen(next_code) + 1);
    strcat(func_code, next_code);
  }

  // account for 128-byte stack red-zone (see notes)
  // allocate stack space as necessary
  // ********************************************
  // TODO: figure out a way to tell if function is a leaf function
  // if this applies, we need to add a "leave ret" as well
  /*
  if (strcmp(scope->scope_id, "main") == 0) {
    char *subrsp_template = "sub rsp, 0x%x\n";
    size_t subrsp_sz = strlen(subrsp_template) + 24;
    code = realloc(code, strlen(code) + subrsp_sz);
    snprintf(code+strlen(code), subrsp_sz, subrsp_template, scope->symtab->offset + scope->symtab->offset % 8);
  }
  */

  code = realloc(code, strlen(code) + strlen(func_code) + 1);
  strcat(code, func_code);
  return code;
}

char *asm_return(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_return()\n");
  size_t sz;
  Scope_t *scope = scope_getcurrentscope(ctx->scope_manager);
  char *ret_reg = type_size(scope->specs_type) == 8 ? "rax" : "eax";
  char *code, *template;
  switch(ctx->node->value->node_type) {
    case AST_NUM:
      template = "mov %s, 0x%x\n";
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, ret_reg, ctx->node->value->num_value);
      break;
    case AST_ID:
      SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, ctx->node->value->name);
      switch (entry->size) {
        case 1: template = "mov %s, byte [rbp-0x%x]\n";   break;
        case 2: template = "mov %s, word [rbp-0x%x]\n";   break;
        case 4: template = "mov %s, dword [rbp-0x%x]\n";  break;
        case 8: template = "mov %s, qword [rbp-0x%x]\n";  break;
        default: error_exit("asm_return() - case AST_ID: size check default reached\n");
      }
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, ret_reg, entry->offset);
      break;
    case AST_BINOP:
      BinopResult_t *res = binop_evaluate(ctx, ctx->node->value);
      if (res->computed) {
        template = "mov %s, 0x%x\n";
        sz = strlen(template) + 24;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, template, ret_reg, res->value);
      }
      else {
        code = res->code;
        char *result_reg = get_used_register(ctx->reg_manager, 1, type_size(scope->specs_type));
        template = "mov %s, %s\n";
        sz = strlen(template) + 8;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template, ret_reg, result_reg);
        free_register(ctx->reg_manager, result_reg);
      }
      break;
    case AST_CALL:
      code = asm_call(ctx, ctx->node->value);
      break;
    default:
      error_exit("asm_return - default case - implement\n");
  }
  char *poprbpret = "pop rbp\nret\n\n";
  code = realloc(code, strlen(code) + strlen(poprbpret) + 1);
  strcat(code, poprbpret);
  return code;
}

char *asm_assignment(AsmCtx_t *ctx)
{
  if (ASM_DEBUG) printf("asm_assignment()\n");
  char *id = ctx->node->decl == NULL ? ctx->node->name : ctx->node->decl->name;
  SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, id);
  char *code, template[64] = {0};
  size_t sz;
  // template for AST_NUM & AST_BINOP - AST_ID uses proxy_template
  switch (entry->size) {
    case 1: strcpy(template, "mov byte [rbp-0x%x], ");  break;
    case 2: strcpy(template, "mov word [rbp-0x%x], ");  break;
    case 4: strcpy(template, "mov dword [rbp-0x%x], "); break;
    case 8: strcpy(template, "mov qword [rbp-0x%x], "); break;
    default: error_exit("asm_assignment() - size check default reached\n");
  }

  AST_t *assigned_node = ctx->node->value;
  switch (assigned_node->node_type) {
    case AST_NUM:
      strcat(template, "0x%x\n");
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, entry->offset, assigned_node->num_value);
      break;
    case AST_ID:
      // a proxy register has to be used
      SymtabEntry_t *assigned_entry = scope_getsymtabentry(ctx->scope_manager, assigned_node->name);
      char *proxy_reg = assigned_entry->size == 8 ? "rax" : "eax";
      char *assign_reg = entry->size == 8 ? "rax" : "eax";
      char *proxy_template;
      switch (assigned_entry->size) {
        case 1: proxy_template = "mov %s, byte [rbp-0x%x]\nmov byte [rbp-0x%x], %s\n";    break;
        case 2: proxy_template = "mov %s, word [rbp-0x%x]\nmov word [rbp-0x%x], %s\n";    break;
        case 4: proxy_template = "mov %s, dword [rbp-0x%x]\nmov dword [rbp-0x%x], %s\n";  break;
        case 8: proxy_template = "mov %s, qword [rbp-0x%x]\nmov qword [rbp-0x%x], %s\n";  break;
        default: error_exit("asm_assignment - AST_ID default case reached\n");
      }
      sz = strlen(proxy_template) + 48;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, proxy_template, proxy_reg, assigned_entry->offset, entry->offset, assign_reg);
      break;
    case AST_BINOP:
      BinopResult_t *res = binop_evaluate(ctx, assigned_node);
      if (res->computed) {
        strcat(template, "0x%x\n");
        sz = strlen(template) + 24;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, template, entry->offset, res->value);
      }
      else {
        code = res->code;
        char *result_reg = get_used_register(ctx->reg_manager, 1, entry->size);
        strcat(template, "%s\n");
        sz = strlen(template) + 24;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template, entry->offset, result_reg);
        free_register(ctx->reg_manager, result_reg);
      }
      break;
    default:
      error_exit("asm_assignment() - default reached\n");
  }
  return code;
}

char *asm_call(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_call()\n");
  char *code = calloc(1, sizeof(char));
  char *template;
  size_t sz;
  Scope_t *call_scope = scope_getscopebyid(ctx->scope_manager, node->name);
  // put args in registers
  List_t *args = node->children;
  for (int i = 0; i < args->size; ++i) {
    AST_t *arg = args->items[i];                  // arg   -> value passed to function
    AST_t *param = call_scope->params->items[i];  // param -> declared in funcdef
    char *reg = get_arg_register(i, type_size(param->specs_type));
    switch (arg->node_type) {
      case AST_NUM:
        template = "mov %s, 0x%x\n";
        sz = strlen(template) + 24;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template, reg, arg->num_value);
        break;
      case AST_ID:
        SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, arg->name);
        switch (entry->size) {
          case 1: template = "mov %s, byte [rbp-0x%x]\n";  break;
          case 2: template = "mov %s, word [rbp-0x%x]\n";  break;
          case 4: template = "mov %s, dword [rbp-0x%x]\n"; break;
          case 8: template = "mov %s, qword [rbp-0x%x]\n"; break;
          default: error_exit("asm_call() - param size check default reached\n");
        }
        sz = strlen(code) + strlen(template) + 36;
        code = realloc(code, sz);
        snprintf(code, sz, template, reg, entry->offset);
        break;
      default: error_exit("asm_call - default case reached\n");
    }
  }
  char *call_template = "call %s\n";
  sz = strlen(call_template) + 64;
  code = realloc(code, strlen(code) + sz);
  snprintf(code+strlen(code), sz, call_template, node->name);
  return code;
}

BinopResult_t *binop_evaluate(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("binop_evaluate()\n");
  assert(node->node_type == AST_BINOP && "binop_evaluate() - node is not a binop");
  printf("eval node op = %s\n", token_kind_to_str(node->op));
  BinopResult_t *res = calloc(1, sizeof(BinopResult_t));
  res->computed = binop_iscomputable(node);
  if (res->computed) {
    res->value = binop_evaluate_(node);
    return res;
  }
  res->code = calloc(1, sizeof(char));
  binop_gen_code(ctx, node, res);
  return res;
}

// if binop consists only of numeric literals,
// compute result on the spot
bool binop_iscomputable(AST_t *node)
{
  if (node->node_type == AST_NUM)
    return true;
  if (node->node_type == AST_BINOP)
    return binop_iscomputable(node->left) && binop_iscomputable(node->right);
  return false;
}

// if binop is computable, use this
long binop_evaluate_(AST_t *node)
{
  if (ASM_DEBUG) printf("binop_evaluate_()\n");
  long x = 0;
  switch (node->node_type) {
    case AST_NUM: return node->num_value;
    case AST_BINOP:
      switch (node->op) {
        case TOKEN_PLUS:  
          x = binop_evaluate_(node->left) + binop_evaluate_(node->right);
          break;
        case TOKEN_MINUS: 
          x = binop_evaluate_(node->left) - binop_evaluate_(node->right);
          break;
        case TOKEN_MUL:   
          x = binop_evaluate_(node->left) * binop_evaluate_(node->right);
          break;
        case TOKEN_DIV:   
          x = binop_evaluate_(node->left) / binop_evaluate_(node->right);
          break;
        default: error_exit("binop_evaluate_() - unknown op\n");
      }
      break;
    default: error_exit("binop_evaluate_() - default reached\n");
  }
  printf("x = %ld\n", x);
  return x;
  //return 0;
}

// post-order traversal of the AST
// store code in BinopResult_t->code
void binop_gen_code(AsmCtx_t *ctx, AST_t *node, BinopResult_t *res)
{
  if (node == NULL) return;
  binop_gen_code(ctx, node->left, res);
  binop_gen_code(ctx, node->right, res);
  char *next_code;
  char *template;
  size_t sz;
  char *regA, *regB;
  switch (node->node_type) {
    case AST_NUM:
      template = "mov %s, 0x%x\n";
      sz = strlen(template) + 24;
      next_code = calloc(sz, sizeof(char));
      snprintf(next_code, sz, template, get_register(ctx->reg_manager, 8), node->num_value);
      break;
    case AST_ID:
      SymtabEntry_t *entry = scope_getsymtabentry(ctx->scope_manager, node->name);
      switch (entry->size) {
        case 1: template = "mov %s, byte [rbp-0x%x]\n";   break;
        case 2: template = "mov %s, word [rbp-0x%x]\n";   break;
        case 4: template = "mov %s, dword [rbp-0x%x]\n";  break;
        case 8: template = "mov %s, qword [rbp-0x%x]\n";  break;
        default: error_exit("binop_gen_code() - AST_ID entry->size default reached\n");
      }
      sz = strlen(template) + 24;
      next_code = calloc(sz, sizeof(char));
      snprintf(next_code, sz, template, get_register(ctx->reg_manager, entry->size), entry->offset);
      break;
    case AST_BINOP:
      switch (node->op) {
        case TOKEN_PLUS:  template = "add %s, %s\n";  break;
        case TOKEN_MINUS: template = "sub %s, %s\n";  break;
        case TOKEN_MUL:   template = "imul %s, %s\n"; break;
        case TOKEN_DIV:   template = "idiv %s, %s\n"; break;
        default: error_exit("binop_get_code() - AST_BINOP probably implement something\n");
      }
      sz = strlen(template) + 16;
      next_code = calloc(sz, sizeof(char));
      // TODO: fix hardcode 8-byte size (?)
      // is there a point to use the lower 4-byte of registers here ?
      regA = get_used_register(ctx->reg_manager, 2, 8);
      regB = get_used_register(ctx->reg_manager, 1, 8);
      snprintf(next_code, sz, template, regA, regB);
      free_register(ctx->reg_manager, regB);
      break;
    case AST_CALL:
      next_code = asm_call(ctx, node);
      Scope_t *call_scope = scope_getscopebyid(ctx->scope_manager, node->name);
      char *ret_reg = type_size(call_scope->specs_type) == 8 ? "rax" : "eax";
      regA = get_register(ctx->reg_manager, type_size(call_scope->specs_type));
      template = "mov %s, %s\n";
      sz = strlen(template) + 8;
      char *mov_result = calloc(sz, sizeof(char));
      snprintf(mov_result, sz, template, regA, ret_reg);
      next_code = realloc(next_code, strlen(next_code) + sz);
      strcat(next_code, mov_result);
      break;
    default: error_exit("binop_get_code() - probably implement something\n");
  }
  res->code = realloc(res->code, strlen(res->code) + strlen(next_code) + 1);
  strcat(res->code, next_code);
}