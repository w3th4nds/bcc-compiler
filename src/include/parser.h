#ifndef _PARSER_H_
#define _PARSER_H_

#include "keywords.h"
#include "token.h"
#include "types.h"
#include "lexer.h"
#include "AST.h"
#include "list.h"
#include "scope.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  Lexer_t *lexer;
  ScopeManager_t *scope_manager;
  Token_t *token;
} Parser_t;

Parser_t *init_parser(Lexer_t *lexer, ScopeManager_t *scope_manager);
Token_t *parser_eat(Parser_t *parser, TokenKind_t type);
Token_t *parser_peek(Parser_t *parser, int offset);
char *parser_get_value(Parser_t *parser);
AST_t *parser_parse(Parser_t *parser);
AST_t *parser_parse_id(Parser_t *parser);
AST_t *parser_parse_call(Parser_t *parser);
AST_t *parser_parse_while(Parser_t *parser);
List_t *parser_parse_list(Parser_t *parser);
AST_t *parser_parse_comp_list(Parser_t *parser);
AST_t *parser_parse_expr(Parser_t *parser);
AST_t *parser_parse_condition(Parser_t *parser);
AST_t *parser_parse_term(Parser_t *parser);
AST_t *parser_parse_assignment(Parser_t *parser);
AST_t *parser_parse_factor(Parser_t *parser);
AST_t *parser_parse_compound(Parser_t *parser);
AST_t *parser_parse_statement(Parser_t *parser);
List_t *parser_parse_params(Parser_t *parser);
AST_t *parser_parse_specs(Parser_t *parser);
AST_t *parser_parse_decl(Parser_t *parser, bool add_to_symtab);
AST_t *parser_parse_function(Parser_t *parser);
bool is_function_def(Parser_t *parser);
bool is_function_call(Parser_t *parser);
bool is_assignment(Parser_t *parser);
bool is_decl(Parser_t *parser);
bool is_while(Parser_t *parser);

#endif
