#include "include/util.h"

/*

This code is not being used and does not work.
Will fix at some point.

*/

char *format(AST_t *node)
{
  char *buf = calloc(50, sizeof(char));
  switch (node->node_type) {
    case AST_NUM:
      snprintf(buf, 50, "NUM: %ld\n", node->num_value);
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
void create_ast_file(AST_t *binop)
{
  printf("type = %s\n", AST_type_to_str(binop->node_type));
  List_t *list = init_list(sizeof(char *));
  list_push(list, format(binop));

  List_t *queue = init_list(sizeof(AST_t *));
  list_push(queue, binop);

  int cnt = 0;
  while (queue->size) {
    cnt++;
    AST_t *node = (AST_t *)list_getitem(queue, 0);
    list_pop_first(queue);
    if (node == NULL) continue;
    if (node->node_type == AST_NUM) {
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
  printf("total nodes = %d\n", cnt);
  
  char *path = "pyutil/AST_BFS.txt";
  FILE *fp;
  if ((fp = fopen(path, "w")) == NULL) {
    printf("Error opening %s\n", path);
    exit(1);
  }

  for (int i = 0; i < list->size; ++i)
    fwrite(list->items[i], sizeof(char), strlen(list->items[i]), fp);

  fclose(fp);
}
