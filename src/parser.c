#include "include/parser.h"

Parser_t *init_parser(Lexer_t *lexer, ScopeManager_t *scope_manager)
{
  Parser_t *parser = calloc(1, sizeof(Parser_t));
  parser->lexer = lexer;
  parser->scope_manager = scope_manager;
  parser->token = lexer_next_token(lexer);
  return parser;
}

AST_t *parser_parse(Parser_t *parser)
{
  AST_t *ast = init_ast(AST_COMPOUND);
  while (parser->token->kind != TOKEN_EOF) {
    if (PARSE_DEBUG) { printf("parser_parse() - current token: "); print_token(parser->token); }
    scope_set(parser->scope_manager, NULL); // reset to global
    if (is_function_def(parser))
      list_push(ast->children, parser_parse_function(parser));
    else
      list_push(ast->children, parser_parse_statement(parser));

    if (parser->token->kind == TOKEN_SEMI)
      parser_eat(parser, TOKEN_SEMI);
  }
  return ast;
}

Token_t *parser_peek(Parser_t *parser, int offset)
{
  if (offset == 0) return parser->token;
  lexer_save(parser->lexer);
  Token_t *peek;
  for (int i = 0; i < offset; ++i)
    peek = lexer_next_token(parser->lexer);
  lexer_rollback(parser->lexer);
  return peek;
}

char *parser_get_value(Parser_t *parser)
{
  char *value = calloc(strlen(parser->token->value)+1, sizeof(char));
  strcpy(value, parser->token->value);
  return value;
}

Token_t *parser_eat(Parser_t *parser, TokenKind kind)
{
  if (PARSE_DEBUG) printf("Eating: \"%s\" %s\n", parser->token->value, token_kind_to_str(parser->token->kind));
  if (parser->token->kind != kind) {
    printf("Parser unexpected token\n"
           "   Expecting:\t%s"
           "   But got:\t", token_kind_to_str(kind));
    print_token(parser->token);
    exit(1);
  }
  parser->token = lexer_next_token(parser->lexer);
  return parser->token;
}

AST_t *parser_parse_id(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_id()\n");
  char *value = parser_get_value(parser);
  parser_eat(parser, TOKEN_ID);
  AST_t *ast = init_ast(AST_ID);
  ast->name = value;
  return ast;
}

AST_t *parser_parse_string(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_string()\n");
  // implement
  return NULL;
}

AST_t *parser_parse_num(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_num()\n");
  char *value = parser_get_value(parser);
  parser_eat(parser, TOKEN_NUM);
  AST_t *ast = init_ast(AST_NUM);
  ast->num_value = strncasecmp(value, "0x", 2) == 0 ? strtol(value, NULL, 16) : atoi(value);
  return ast;
}

AST_t *parser_parse_return(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_return()\n");
  parser_eat(parser, TOKEN_RETURN);
  AST_t *ast = init_ast(AST_RETURN);
  ast->value = parser_parse_expr(parser);
  return ast;
}

AST_t *parser_parse_statement(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_statement()\n");
  if (parser->token->kind == TOKEN_RETURN) return parser_parse_return(parser);
  if (is_function_call(parser)) return parser_parse_call(parser);
  if (is_assignment(parser))    return parser_parse_assignment(parser);
  if (is_decl(parser))          return parser_parse_decl(parser, true);
  if (is_while(parser))         return parser_parse_while(parser);
  error_exit("parser_parse_statement() - implement\n");
  return NULL;
}

AST_t *parser_parse_assignment(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_assignment()\n");
  AST_t *ast = init_ast(AST_ASSIGNMENT);
  if (is_decl(parser)) {
    ast->decl = parser_parse_decl(parser, true);
  }
  else {
    ast->name = parser_get_value(parser);
    parser_eat(parser, TOKEN_ID);
  }
  ast->op = parser->token->kind;
  parser_eat(parser, parser->token->kind);
  ast->value = parser_parse_expr(parser);
  return ast;
}

AST_t *parser_parse_call(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_call()\n");
  AST_t *ast = init_ast(AST_CALL);
  // id
  ast->name = parser_get_value(parser);
  parser_eat(parser, TOKEN_ID);
  // args
  ast->args = parser_parse_list(parser);
  // since the current function is making a call
  // its no longer a leaf function
  // (ignored by global scope)
  scope_unset_leaf(parser->scope_manager);
  return ast;
}

AST_t *parser_parse_while(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_while()\n");
  parser_eat(parser, TOKEN_WHILE);
  parser_eat(parser, TOKEN_LP);
  AST_t *ast = init_ast(AST_WHILE);
  ast->cond = parser_parse_condition(parser);
  parser_eat(parser, TOKEN_RP);
  ast->body = parser_parse_compound(parser);
  return ast;
}

List_t *parser_parse_list(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_list()\n");
  parser_eat(parser, TOKEN_LP);
  List_t *list = init_list(sizeof(AST_t *));
  if (parser->token->kind != TOKEN_RP) {
    list_push(list, parser_parse_expr(parser));
    while (parser->token->kind == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      list_push(list, parser_parse_expr(parser));
    }
  }
  parser_eat(parser, TOKEN_RP);
  return list;
}

AST_t *parser_parse_comp_list(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_comp_list()\n");
  // TODO: add bracket support
  bool ispar = parser->token->kind == TOKEN_LP;
  parser_eat(parser, ispar ? TOKEN_LP : TOKEN_LBRACE);

  AST_t *ast = init_ast(AST_COMPOUND);
  if (parser->token->kind != (ispar ? TOKEN_RP : TOKEN_RBRACE)) {
    list_push(ast->children, parser_parse_expr(parser));
    while (parser->token->kind == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      list_push(ast->children, parser_parse_expr(parser));
    }
  }
  parser_eat(parser, ispar ? TOKEN_RP : TOKEN_RBRACE);
  return ast;
}

AST_t *parser_parse_factor(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_factor()\n");
  switch (parser->token->kind) {
    case TOKEN_LP:      
      parser_eat(parser, TOKEN_LP);
      AST_t *par_expr = parser_parse_expr(parser);
      parser_eat(parser, TOKEN_RP);
      return par_expr;
    case TOKEN_NUM:     
      return parser_parse_num(parser);
    case TOKEN_STRING:  
      return parser_parse_string(parser);
    case TOKEN_ID:      
      return is_function_call(parser) ? parser_parse_call(parser) : parser_parse_id(parser);
    default:
      print_token(parser->token);
      error_exit("parser_parse_factor: Implement\n");
  }
  return NULL;
}

AST_t *parser_parse_term(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_term()\n");
  AST_t *ast = parser_parse_factor(parser);
  while (parser->token->kind == TOKEN_MUL || parser->token->kind == TOKEN_DIV) {
    AST_t *tmp_ast = ast;
    ast = init_ast(AST_BINOP);
    ast->left = tmp_ast;
    ast->op = parser->token->kind;
    parser_eat(parser, parser->token->kind);
    ast->right = parser_parse_term(parser);
  }
  return ast;
}

AST_t *parser_parse_expr(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_expr()\n");
  AST_t *ast = parser_parse_term(parser);
  while (parser->token->kind == TOKEN_PLUS || parser->token->kind == TOKEN_MINUS) {
    AST_t *tmp_ast = ast;
    ast = init_ast(AST_BINOP);
    ast->left = tmp_ast;
    ast->op = parser->token->kind;
    parser_eat(parser, parser->token->kind);
    ast->right = parser_parse_expr(parser);
  }
  return ast;
}

// just parser_parser_expr wrapper
// with conditionals support
// TODO: add "||" "&&" support -> they need a higher lvl function
AST_t *parser_parse_condition(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_condition()\n");
  AST_t *ast = parser_parse_expr(parser);
  while (is_cond_op(parser->token->kind)) {
    AST_t *tmp_ast = ast;
    ast = init_ast(AST_COND);
    ast->left = tmp_ast;
    ast->op = parser->token->kind;
    parser_eat(parser, parser->token->kind);
    ast->right = parser_parse_condition(parser);
  }
  return ast;
}

AST_t *parser_parse_compound(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_compound()\n");
  if (parser->token->kind == TOKEN_LBRACE)
    parser_eat(parser, TOKEN_LBRACE);

  AST_t *compound = init_ast(AST_COMPOUND);
  while (parser->token->kind != TOKEN_EOF && parser->token->kind != TOKEN_RBRACE) {
    list_push(compound->children, parser_parse_statement(parser));
    if (parser->token->kind == TOKEN_SEMI)
      parser_eat(parser, TOKEN_SEMI);
  }
  if (parser->token->kind == TOKEN_RBRACE)
    parser_eat(parser, TOKEN_RBRACE);

  return compound;
}

List_t *parser_parse_params(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_params()\n");
  parser_eat(parser, TOKEN_LP);
  List_t *list = init_list(sizeof(AST_t *));
  if (parser->token->kind != TOKEN_RP) {
    list_push(list, parser_parse_decl(parser, false));
    while (parser->token->kind == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      list_push(list, parser_parse_decl(parser, false));
    }
  }
  parser_eat(parser, TOKEN_RP);
  return list;
}

AST_t *parser_parse_decl(Parser_t *parser, bool add_to_symtab)
{
  if (PARSE_DEBUG) printf("parser_parse_decl()\n");
  AST_t *ast = init_ast(AST_DECL);
  // specs
  ast->specs_type = match_type(parser->token->kind);
  if (ast->specs_type == NO_TYPE)
    error_exit("parser_parse_decl() - NO_TYPE error\n");
  parser_eat(parser, parser->token->kind);
  // void is not followed by an id
  if (ast->specs_type == TYPE_VOID) return ast;
  // id
  ast->name = parser_get_value(parser);
  parser_eat(parser, TOKEN_ID);
  // add to scope
  if (add_to_symtab) scope_add_to_symtab(parser->scope_manager, ast->specs_type, ast->name);
  return ast;
}

AST_t *parser_parse_function(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("parser_parse_function()\n");
  AST_t *ast = init_ast(AST_FUNCTION);
  // function specs
  ast->specs_type = match_type(parser->token->kind);
  if (ast->specs_type == NO_TYPE) error_exit("parser_parse_function - specs_type is NO_TYPE\n");
  parser_eat(parser, parser->token->kind);
  // function id
  ast->name = parser_get_value(parser);
  parser_eat(parser, TOKEN_ID);
  scope_set(parser->scope_manager, ast->name);
  // function params
  ast->params = parser_parse_params(parser);
  scope_add_specs(parser->scope_manager, ast->specs_type, ast->params);
  // function body
  ast->body = parser_parse_compound(parser);
  return ast;
}

// TODO: cleanup recon functions
bool is_function_def(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("is_function_def()\n");
  int state = 0, idx = 0;
  for (;;) {
    Token_t *tok = parser_peek(parser, idx++);
    if (tok->kind == TOKEN_EOF)
      error_exit("is_function_def() - premature EOF found");
    switch (state) {
      case 0:
        if (match_type(tok->kind) != NO_TYPE) { state++; continue; }
        break;
      case 1:
        if (tok->kind == TOKEN_ID) { state++; continue; }
        break;;
      case 2:
        if (tok->kind == TOKEN_LP) { state++; continue; }
        break;
      case 3:
        if (tok->kind == TOKEN_RP) state++;
        continue;
      case 4:
        if (tok->kind == TOKEN_LBRACE) { state++; continue; }
      case 5:
      default:
    }
    break;
  }
  return state == 5;
}

bool is_assignment(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("is_assignment()\n");
  int state = 0, idx = 0;
  for (;;) {
    Token_t *tok = parser_peek(parser, idx++);
    if (tok->kind == TOKEN_EOF)
      error_exit("is_assignment() - premature EOF found");
    switch(state) {
      case 0:
        if (match_type(tok->kind) != NO_TYPE) { state++; continue; }
        state++;
      case 1:
        if (tok->kind == TOKEN_ID) { state++; continue; }
        break;
      case 2:
        if (is_assign_op(tok->kind)) { state++; continue; }
        break;
      case 3:
      default:
    }
    break;
  }
  return state == 3;
}

bool is_function_call(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("is_function_call()\n");
  int state = 0, idx = 0;
  for (;;) {
    Token_t *tok = parser_peek(parser, idx++);
    if (tok->kind == TOKEN_EOF)
      error_exit("is_function_call() - premature EOF found");
    switch(state) {
      case 0:
        if (tok->kind == TOKEN_ID) { state++; continue; }
        break;
      case 1:
        if (tok->kind == TOKEN_LP) { state++; continue; }
        break;
      case 2: // TODO: clean up - add func call as arg recon 
        if (tok->kind == TOKEN_ID || tok->kind == TOKEN_NUM || tok->kind == TOKEN_COMMA) continue;
        state++;
      case 3:
        if (tok->kind == TOKEN_RP) { state++; continue; }
        break;
      case 4:
      default:
    }
    break;
  }
  return state == 4;
}

bool is_decl(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("is_decl()\n");
  if (match_type(parser_peek(parser, 0)->kind) != NO_TYPE && \
      parser_peek(parser, 1)->kind == TOKEN_ID && \
      (parser_peek(parser, 2)->kind == TOKEN_SEMI || 
       is_assign_op(parser_peek(parser, 2)->kind))) return true;
  return false;
}

// TODO: cleanup
bool is_while(Parser_t *parser)
{
  if (PARSE_DEBUG) printf("is_while()\n");
  if (parser_peek(parser, 0)->kind == TOKEN_WHILE && \
      parser_peek(parser, 1)->kind == TOKEN_LP) return true;
  return false;
}