#include "include/util.h"

// TODO: expand this to make it print out entire source code AST, not just expressions

char *format(AST_t *node)
{
  char *buf = calloc(50, sizeof(char));
  switch (node->node_type) {
    case AST_NUM:
      snprintf(buf, 50, "NUM: %ld\n", node->num_value);
      break;
    case AST_ID:
      snprintf(buf, 50, "ID: %s\n", node->name);
      break;
    case AST_CALL:
      snprintf(buf, 50, "CALL: %s\n", node->name);
      break;
    case AST_BINOP:
      char op[2] = {0};
      switch (node->op)
      {
        case TOKEN_PLUS:  strcpy(op, "+"); break;
        case TOKEN_MINUS: strcpy(op, "-"); break;
        case TOKEN_MUL:   strcpy(op, "*"); break;
        case TOKEN_DIV:   strcpy(op,"/");  break;
      }
      snprintf(buf, 50, "BINOP: \"%s\"\n", op);
      break;
    default: error_exit("util.c format() - default reached\n");
  }
  return buf;
}

// perform level order traversal on the root node of the AST
// to be used for visualization with python
// TODO: clean up

void create_ast_file(AST_t *expr, int expr_n)
{
  List_t *list = init_list(sizeof(char *));
  list_push(list, format(expr));

  List_t *queue = init_list(sizeof(AST_t *));
  list_push(queue, expr);

  int cnt = 0;
  while (queue->size) {
    cnt++;
    AST_t *node = (AST_t *)list_getitem(queue, 0);
    list_pop_first(queue);
    if (node == NULL) continue;
    if (node->node_type != AST_BINOP) {
      char *buf = calloc(6, sizeof(char));
      strcpy(buf, "NULL\n");
      list_push(list, buf);
      list_push(list, buf);
    }
    else {
      list_push(list, format(node->left));
      list_push(list, format(node->right));
    }
    list_push(queue, node->left);
    list_push(queue, node->right);
  }
  char *path_ = "pyutil/AST_BFS_%d.txt";
  char path[36] = {0};
  snprintf(path, 36, path_, expr_n);
  FILE *fp;
  if ((fp = fopen(path, "w")) == NULL) {
    printf("Error opening %s\n", path);
    exit(1);
  }

  for (int i = 0; i < list->size; ++i)
    fwrite(list->items[i], sizeof(char), strlen(list->items[i]), fp);

  fclose(fp);
}
