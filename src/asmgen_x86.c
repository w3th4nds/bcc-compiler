#include "include/asmgen_x86.h"

// TODO: rewrite asm gen
// too much spaghetti code
// think architecture through

AsmCtx_t *init_asm_ctx(ScopeManager_t *scope_manager)
{
  AsmCtx_t *ctx = calloc(1, sizeof(AsmCtx_t));
  ctx->scope_manager = scope_manager;
  ctx->reg_manager = init_register_manager();
  return ctx;
}

char *asm_generate(ScopeManager_t *scope_manager, AST_t *root)
{
  if (ASM_DEBUG) printf("asm_generate()\n");
  assert(root != NULL && root->node_type == AST_COMPOUND);
  AsmCtx_t *ctx = init_asm_ctx(scope_manager);

  char *code = calloc(2, sizeof(char));
  // declare functions as global
  List_t *ids = hashmap_get_all_ids(scope_manager->scopes);
  for (int i = 0; i < ids->size; ++i) {
    char *global_template = "global %s\n";
    size_t global_template_sz = strlen(global_template) + strlen(ids->items[i]) + 1;
    code = realloc(code, strlen(code) + global_template_sz);
    snprintf(code+strlen(code), global_template_sz, global_template, (char *)ids->items[i]);
  }

  // .text section
  char *textsect = "\nsection .text\n\n";
  size_t newsz = strlen(code) + strlen(textsect) + 1;
  code = realloc(code, newsz);
  strcat(code, textsect);

  char *next_code;
  for (int i = 0; i < root->children->size; ++i) {
    AST_t *node = root->children->items[i];
    scope_set(ctx->scope_manager, NULL); // reset scope to global
    switch (node->node_type) {
      case AST_FUNCTION: next_code = asm_func_def(ctx, node); break;
      case AST_ASSIGNMENT:
      case AST_DECL: break;
      default:
        printf("node_type = %s\n", AST_type_to_str(node->node_type));
        error_exit("asm_generate() - default reached\n");
    }
    code = realloc(code, strlen(code) + strlen(next_code) + 1);
    strcat(code, next_code);
    //printf("code so far = \n%s\n", code);
  }
  return code;
}

char *asm_nop(void)
{
  return "";
}

char *asm_access(AsmCtx_t *ctx, AST_t *node)
{
  char *code, *template;
  size_t sz;
  switch (node->node_type) {
    case AST_NUM:
      template = "0x%x";
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, node->num_value);
      break;
    case AST_DECL:
    case AST_ASSIGNMENT:
    case AST_ID:
      Variable_t *var = scope_getvariable(ctx->scope_manager, node->name);
      switch (var->scope) {
        case VAR_LOCAL:
          switch (var->size) {
            case sizeof(char):  template = "byte [rbp-0x%x]";  break;
            case sizeof(short): template = "word [rbp-0x%x]";  break;
            case sizeof(int):   template = "dword [rbp-0x%x]"; break;
            case sizeof(long):  template = "qword [rbp-0x%x]"; break;
          }
          sz = strlen(template) + 24;
          code = calloc(sz, sizeof(char));
          snprintf(code, sz, template, var->offset);
          break;
        case VAR_PARAM:
          template = "%s";
          sz = strlen(template) + 8;
          code = calloc(sz, sizeof(char));
          snprintf(code, sz, template, get_arg_register(var->param_n, var->size));
          break;
        case VAR_GLOBAL:
          printf("asm_access() - implement VAR_GLOBAL\n");
        default:
          error_exit("asm_access() - unknown var->scope\n");
      }
      break;
    default:
      printf("node type = %s\n", AST_type_to_str(node->node_type));
      error_exit("asm_access() - deafult reached\n");
  }
  return code;
}

char *asm_func_def(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_func_def()\n");
  assert(node != NULL && node->node_type == AST_FUNCTION);
  // set the scope to make things easier
  scope_set(ctx->scope_manager, node->name);
  Scope_t *scope = hashmap_getscope(ctx->scope_manager->scopes, node->name);
  char *template = "%s:\npush rbp\nmov rbp, rsp\n";
  size_t sz = strlen(template) + 64;
  char *code = calloc(sz, sizeof(char));
  snprintf(code, sz, template, scope->scope_id);

  // parse function body
  char *func_code = asm_compound(ctx, node->body);

  // account for stack red-zone (see notes)
  // allocate stack space as necessary
  if (scope->is_leaf && scope->symtab->offset <= STACK_REDZONE_SIZE) {
    // pop rbp ret
    char *poprbpret = "pop rbp\nret\n\n";
    func_code = realloc(func_code, strlen(func_code) + strlen(poprbpret) + 1);
    strcat(func_code, poprbpret);
  }
  else {
    // sub rsp
    char *subrsp_template = "sub rsp, 0x%x\n";
    size_t subrsp_sz = strlen(subrsp_template) + 24;
    code = realloc(code, strlen(code) + subrsp_sz);
    snprintf(code+strlen(code), subrsp_sz, subrsp_template, scope->symtab->offset + scope->symtab->offset % 16);
    // leave ret
    char *leaveret = "leave\nret\n\n";
    func_code = realloc(func_code, strlen(func_code) + strlen(leaveret) + 1);
    strcat(func_code, leaveret);
  }

  // put everything together
  code = realloc(code, strlen(code) + strlen(func_code) + 1);
  strcat(code, func_code);
  return code;
}

char *asm_statement(AsmCtx_t *ctx, AST_t *node)
{
  switch (node->node_type) {
    case AST_NOP:         return asm_nop();
    case AST_RETURN:      return asm_return(ctx, node);
    case AST_ASSIGNMENT:  return asm_assignment(ctx, node);
    case AST_CALL:        return asm_call(ctx, node);
    case AST_WHILE:       return asm_while(ctx, node);
    case AST_FOR:         return asm_for(ctx, node);
    case AST_DECL:        return ""; // does nothing in asm phase
    default:
      printf("child node_type = %s\n", AST_type_to_str(node->node_type));
      error_exit("asm_statment() - default reached\n");
  }
  return NULL;
}

char *asm_compound(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_compound()\n");
  assert(node != NULL && node->node_type == AST_COMPOUND);
  char *code = calloc(1, sizeof(char));
  List_t *children = node->children;
  for (int i = 0; i < children->size; ++i) {
    AST_t *child = children->items[i];
    char *next_code = asm_statement(ctx, child);
    code = realloc(code, strlen(code) + strlen(next_code) + 1);
    strcat(code, next_code);
  }
  return code;
}

char *asm_return(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_return()\n");
  assert(node != NULL && node->node_type == AST_RETURN);
  size_t sz;
  Scope_t *scope = scope_getcurrentscope(ctx->scope_manager);
  char *ret_reg = type_size(scope->specs_type) == 8 ? "rax" : "eax";
  char *code, *template;
  switch(node->value->node_type) {
    case AST_NUM:
      template = "mov %s, 0x%x\n";
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, ret_reg, node->value->num_value);
      break;
    case AST_ID:
      template = "mov %s, %s\n";
      sz = strlen(template) + 32;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, ret_reg, asm_access(ctx, node->value));
      break;
    case AST_BINOP:
      BinopResult_t *res = binop_evaluate(ctx, node->value);
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
      code = asm_call(ctx, node->value);
      break;
    default:
      error_exit("asm_return - default case - implement\n");
  }
  return code;
}

// TODO: account for operation performed
// to support +=, -=, etc
char *asm_assignment(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_assignment()\n");
  assert(node != NULL && node->node_type == AST_ASSIGNMENT);
  Variable_t *var = scope_getvariable(ctx->scope_manager, node->name);
  char *code, *template;
  size_t sz;
  AST_t *assigned_node = node->value;
  switch (assigned_node->node_type) {
    case AST_NUM:
      template = "mov %s, 0x%x\n";
      sz = strlen(template) + 24;
      code = calloc(sz, sizeof(char));
      snprintf(code, sz, template, asm_access(ctx, node), assigned_node->num_value);
      break;
    case AST_ID:
      Variable_t *assigned_var = scope_getvariable(ctx->scope_manager, assigned_node->name);
      // special case in locals/globals, a proxy register has to be used
      if (assigned_var->scope == VAR_LOCAL || assigned_var->scope == VAR_GLOBAL) {
        char *proxy_reg = assigned_var->size == 8 ? "rax" : "eax";
        char *assign_reg = var->size == 8 ? "rax" : "eax";
        char *proxy_template = "mov %s, %s\nmov %s, %s\n";
        sz = strlen(proxy_template) + 64;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, proxy_template, proxy_reg, asm_access(ctx, assigned_node), asm_access(ctx, node), assign_reg);
      }
      else {
        template = "mov %s, %s\n";
        sz = strlen(template) + 32;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, template, asm_access(ctx, node), asm_access(ctx, assigned_node));
      }
      break;
    case AST_BINOP:
      BinopResult_t *res = binop_evaluate(ctx, assigned_node);
      if (res->computed) {
        template = "mov %s, 0x%x\n";
        sz = strlen(template) + 24;
        code = calloc(sz, sizeof(char));
        snprintf(code, sz, template, asm_access(ctx, node), res->value);
      }
      else {
        code = res->code;
        char *result_reg = get_used_register(ctx->reg_manager, 1, var->size);
        template = "mov %s, %s\n";
        sz = strlen(template) + 24;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template, asm_access(ctx, node), result_reg);
        free_register(ctx->reg_manager, result_reg);
      }
      break;
    case AST_CALL:
      Scope_t *call_scope = scope_getscopebyid(ctx->scope_manager, assigned_node->name);
      code = asm_call(ctx, assigned_node);
      char *ret_reg = type_size(call_scope->specs_type) == 8 ? "rax" : "eax";
      template = "mov %s, %s\n";
      sz = strlen(template) + 24;
      code = realloc(code, strlen(code) + sz);
      snprintf(code+strlen(code), sz, template, asm_access(ctx, node), ret_reg);
      break;
    default:
      error_exit("asm_assignment() - default reached\n");
  }
  return code;
}

char *asm_call(AsmCtx_t *ctx, AST_t *node)
{
  assert(node != NULL && node->node_type == AST_CALL);
  if (ASM_DEBUG) printf("asm_call()\n");
  char *code = calloc(1, sizeof(char));
  char *template;
  size_t sz;
  Scope_t *call_scope = scope_getscopebyid(ctx->scope_manager, node->name);
  // put args in registers
  List_t *args = node->args;
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
        template = "mov %s, %s\n";
        sz = strlen(template) + 32;
        code = realloc(code, strlen(code) + sz);
        snprintf(code+strlen(code), sz, template, reg, asm_access(ctx, arg));
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

char *asm_while(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_while()\n");
  char *lbl_cond = make_label();
  char *lbl_body = make_label();
  char *template = "jmp %s\n%s:\n%s%s:\n%s";
  // condition
  char *cond_code = asm_condition(ctx, node->cond, lbl_body);
  // body
  char *body_code = asm_compound(ctx, node->body);
  // join them
  size_t sz = strlen(template) + strlen(cond_code) + strlen(body_code) + 32;
  char *code = calloc(sz, sizeof(char));
  snprintf(code, sz, template, lbl_cond, lbl_body, body_code, lbl_cond, cond_code);
  return code;
}

char *asm_for(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("asm_for()\n");
  char *lbl_cond = make_label();
  char *lbl_body = make_label();
  char *template = "%sjmp %s\n%s:\n%s%s%s:\n%s";
  // TODO: 2nd statement doesn't have to be a condition - fix ?
  // statement 1
  char *stmt1_code = asm_statement(ctx, node->stmt1);
  // statement 2 (condition)
  char *cond_code = asm_condition(ctx, node->stmt2, lbl_body);
  // statement 3
  char *stmt3_code = asm_statement(ctx, node->stmt3);
  // body
  char *body_code = asm_compound(ctx, node->body);
  size_t sz = strlen(stmt1_code) + strlen(cond_code) + strlen(stmt3_code) + strlen(body_code) \
              + strlen(lbl_body) + strlen(lbl_cond) + strlen(template) + 1;
  char *code = calloc(sz, sizeof(char));
  snprintf(code, sz, template, stmt1_code, lbl_cond, lbl_body, body_code, stmt3_code, lbl_cond, cond_code);
  return code;
}

char *asm_condition(AsmCtx_t *ctx, AST_t *node, char *jmp_label)
{
  if (ASM_DEBUG) printf("asm_condition()\n");
  assert(node != NULL && node->node_type == AST_COND);
  char *jmp;
  switch (node->op) {
    case TOKEN_EQEQ:  jmp = "je";   break;
    case TOKEN_NOTEQ: jmp = "jne";  break;
    case TOKEN_GT:    jmp = "jg";   break;
    case TOKEN_GE:    jmp = "jge";  break;
    case TOKEN_LT:    jmp = "jl";   break;
    case TOKEN_LE:    jmp = "jle";  break;
    default: error_exit("asm_condition() unknown op\n");
  }

  // TODO: add binop/call support for conditions

  char *template = "cmp %s, %s\n%s %s\n";
  size_t sz = strlen(template) + 88;
  char *code = calloc(sz, sizeof(char));

  snprintf(code+strlen(code), sz, template, asm_access(ctx, node->left), asm_access(ctx, node->right), jmp, jmp_label);
  return code;
}

BinopResult_t *binop_evaluate(AsmCtx_t *ctx, AST_t *node)
{
  if (ASM_DEBUG) printf("binop_evaluate()\n");
  assert(node != NULL && node->node_type == AST_BINOP);
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
  switch (node->node_type) {
    case AST_NUM: return node->num_value;
    case AST_BINOP:
      switch (node->op) {
        case TOKEN_PLUS:  return binop_evaluate_(node->left) + binop_evaluate_(node->right);
        case TOKEN_MINUS: return binop_evaluate_(node->left) - binop_evaluate_(node->right);
        case TOKEN_MUL:   return binop_evaluate_(node->left) * binop_evaluate_(node->right);
        case TOKEN_DIV:   return binop_evaluate_(node->left) / binop_evaluate_(node->right);
        default: error_exit("binop_evaluate_() - unknown op\n");
      }
      break;
    default: error_exit("binop_evaluate_() - default reached\n");
  }
  return 0;
}

// post-order traversal of the AST
// store code in BinopResult_t->code
void binop_gen_code(AsmCtx_t *ctx, AST_t *node, BinopResult_t *res)
{
  if (node == NULL) return;
  binop_gen_code(ctx, node->left, res);
  binop_gen_code(ctx, node->right, res);
  char *next_code, *template;
  size_t sz;
  char *regA, *regB;
  switch (node->node_type) {
    case AST_NUM:
      template = "mov %s, 0x%x\n";
      sz = strlen(template) + 24;
      next_code = calloc(sz, sizeof(char));
      snprintf(next_code, sz, template, get_register(ctx->reg_manager, sizeof(long)), node->num_value);
      break;
    case AST_ID:
      Variable_t *var = scope_getvariable(ctx->scope_manager, node->name);
      template = "mov %s, %s\n";
      sz = strlen(template) + 32;
      next_code = calloc(sz, sizeof(char));
      snprintf(next_code, sz, template, get_register(ctx->reg_manager, var->size), asm_access(ctx, node));
      break;
    case AST_BINOP:
      switch (node->op) {
        case TOKEN_PLUS:  template = "add %s, %s\n";   break;
        case TOKEN_MINUS: template = "sub %s, %s\n";   break;
        case TOKEN_MUL:   template = "imul %s, %s\n";  break;
        case TOKEN_DIV: break; // special case
        default:
          printf("token = %s %s\n", token_kind_to_str(node->op), node->name);
          error_exit("binop_get_code() - AST_BINOP probably implement something\n");
      }
      // "div src": divides rax/src
      // the ratio is put into rax, and the remainder in rdx
      // on input, rdx MUST be zero
      if (node->op == TOKEN_DIV) {
        template = "mov rdx, 0x0\nmov %s, %s\nidiv %s\nmov %s, %s\n";
        sz = strlen(template) + 16;
        next_code = calloc(sz, sizeof(char));
        size_t reg_sz = get_used_register_size(ctx->reg_manager, 1);
        regA = get_used_register(ctx->reg_manager, 2, reg_sz);
        regB = get_used_register(ctx->reg_manager, 1, reg_sz);
        char *regDiv =  reg_sz == 8 ? "rax" : "eax";
        snprintf(next_code, sz, template, regDiv, regA, regB, regA, regDiv);
        free_register(ctx->reg_manager, regB);
        break;
      }
      sz = strlen(template) + 16;
      next_code = calloc(sz, sizeof(char));
      // get the min size of the last 2 used registers
      size_t min_sz = MIN(get_used_register_size(ctx->reg_manager, 1), get_used_register_size(ctx->reg_manager, 2));
      regA = get_used_register(ctx->reg_manager, 2, min_sz);
      regB = get_used_register(ctx->reg_manager, 1, min_sz);
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

char *make_label(void)
{
  static long id = 0;
  char *lbl_template = ".L%ld";
  size_t sz = strlen(lbl_template) + 8;
  char *label = calloc(sz, sizeof(char));
  snprintf(label, sz, lbl_template, id++);
  return label;
}