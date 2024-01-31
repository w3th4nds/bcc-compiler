#include "include/binop_eval.h"

BinopResult_t *binop_evaluate(AST_t *node)
{
  assert(node->node_type == AST_BINOP && "binop_evaluate() - node is not a binop");
  BinopResult_t *res = calloc(1, sizeof(BinopResult_t));
  res->computed = binop_iscomputable(node);
  if (res->computed) res->value = binop_evaluate_(node);
  return res;
}

// traverse tree
bool binop_iscomputable(AST_t *node)
{
  if (node->node_type == AST_NUM)
    return true;
  if (node->node_type == AST_BINOP)
    return binop_iscomputable(node->left) && binop_iscomputable(node->right);
  if (ASM_DEBUG) printf("binop_iscomputable() - node type = %s\n", AST_type_to_str(node->node_type));
  return false;
}

int binop_evaluate_(AST_t *node)
{
  switch (node->node_type) {
    case AST_BINOP:
      if (node->op == TOKEN_PLUS)
        return binop_evaluate_(node->left) + binop_evaluate_(node->right);
      if (node->op == TOKEN_MINUS)
        return binop_evaluate_(node->left) - binop_evaluate_(node->right);
      if (node->op == TOKEN_MUL)
        return binop_evaluate_(node->left) * binop_evaluate_(node->right);
      if (node->op == TOKEN_DIV)
        return binop_evaluate_(node->left) / binop_evaluate_(node->right);
    case AST_NUM:
      printf("astnum = %d\n", node->num_value);
      return node->num_value;
    default:
      error_exit("binop_evaluate_() - default reached\n");
  }
  return 0; // get rid of compiler warnings
}
