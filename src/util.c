#include "include/util.h"

char *format(AST_t *node)
{
  char *buf = calloc(1000, sizeof(char));
  snprintf(buf, 1000, "(%s %s),", node->name, AST_type_to_str(node->node_type));
  if (node->children != NULL) {
    for (int i = 0; i < node->children->size; ++i) {
      AST_t *tmp = node->children->items[i];
      snprintf(buf+strlen(buf), 1000, "(%s %s),", tmp->name, AST_type_to_str(tmp->node_type));
    }
  }
  buf[strlen(buf)] = '\n';
  return buf;
}

// perform level order traversal on the root node of the AST
// to be used for visualization with python
void create_ast_file(AST_t *root)
{
  List_t *list = init_list(sizeof(char *));
  list_push(list, format(root));

  List_t *queue = init_list(sizeof(AST_t *));
  list_push(queue, root);

  int cnt = 0;
  while (queue->size) {
    cnt++;
    AST_t *node = (AST_t *)list_getitem(queue, 0);
    list_pop_first(queue);

    if (node->children == NULL) continue;
    for (int i = 0; i < node->children->size; ++i) {
      list_push(list, format(node->children->items[i]));
      list_push(queue, node->children->items[i]);
    }
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
