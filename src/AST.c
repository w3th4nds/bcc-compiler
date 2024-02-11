#include "include/AST.h"

AST_t *init_ast(AstType_t node_type)
{
  static unsigned long node_id = 0;
  AST_t *ast = calloc(1, sizeof(AST_t));
  ast->node_id = node_id++;
  ast->node_type = node_type;
  if (node_type == AST_COMPOUND) ast->children = init_list(sizeof(AST_t *));
  else if (node_type == AST_CALL) ast->args = init_list(sizeof(AST_t *));
  else if (node_type == AST_FUNCTION) ast->params = init_list(sizeof(AST_t *));
  return ast;
}

char *AST_type_to_str(AstType_t node_type)
{
  switch (node_type) {
    case AST_NOP:         return "AST_NOP";
    case AST_NUM:         return "AST_NUM";
    case AST_ID:          return "AST_ID";
    case AST_DECL:        return "AST_DECL";
    case AST_FUNCTION:    return "AST_FUNCTION";
    case AST_CALL:        return "AST_CALL";
    case AST_ASSIGNMENT:  return "AST_ASSIGNMENT";
    case AST_RETURN:      return "AST_RETURN";
    case AST_BINOP:       return "AST_BINOP";
    case AST_COMPOUND:    return "AST_COMPOUND";
    case AST_COND:        return "AST_COND";
    case AST_WHILE:       return "AST_WHILE";
    case AST_FOR:         return "AST_FOR";
    case AST_IF:          return "AST_IF";
    default:                  
      printf("type = %d\n", node_type);
      return "AST_type_to_str(): type not implemented";
  }
}