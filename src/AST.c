#include "include/AST.h"

AST_t *init_ast(int node_type)
{
  AST_t *ast = calloc(1, sizeof(AST_t));
  ast->node_type = node_type;
  switch (node_type) {
    case AST_COMPOUND:
      ast->children = init_list(sizeof(AST_t *));
      ast->name = "AST_COMPOUND";
      break;
    case AST_RETURN:
      ast->name = "AST_RETURN";
      break;
    case AST_CALL:
      ast->children = init_list(sizeof(AST_t *));
      break;
    default:
  }
  return ast;
}

char *AST_type_to_str(int node_type)
{
  switch (node_type) {
    case AST_NUM:             return "AST_NUM";
    case AST_ID:              return "AST_ID";
    case AST_DECL:            return "AST_DECL";
    case AST_FUNCTION:        return "AST_FUNCTION";
    case AST_CALL:            return "AST_CALL";
    case AST_ASSIGNMENT:      return "AST_ASSIGNMENT";
    case AST_RETURN:          return "AST_RETURN";
    case AST_BINOP:           return "AST_BINOP";
    case AST_COMPOUND:        return "AST_COMPOUND";
    default:                  
      printf("type = %d\n", node_type);
      return "AST_type_to_str(): type not implemented";
  }
}

char *AST_to_str(AST_t *ast)
{
  char *buf = calloc(200, sizeof(char));
  snprintf(buf, 200, "<AST_t type=%s, ", AST_type_to_str(ast->node_type));
  switch (ast->node_type) {
    case AST_COMPOUND:
      snprintf(buf+strlen(buf), 200, "children=%ld>", ast->children->size);
      return buf;
    case AST_FUNCTION:
      //snprintf(buf+strlen(buf), 200, "specs=%d, decl=%s>", AST_type_to_str(ast->node_type), ast->children->size);
    case AST_CALL:
    case AST_ASSIGNMENT:
    case AST_RETURN:
    case AST_BINOP:
    case AST_NUM:
    case AST_ID:
    default:
      return "AST_to_str(): implement";
  }
}
