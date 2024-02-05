#include "include/util.h"

char *format(AST_t *node)
{
  size_t sz = 256;
  char *buf = calloc(sz, sizeof(char));
  snprintf(buf, sz, "[%ld] ", node->node_id);
  switch (node->node_type) {
    case AST_NUM:
      snprintf(buf+strlen(buf), sz, "NUM: %ld,", node->num_value);
      break;
    case AST_ID:
      snprintf(buf+strlen(buf), sz, "ID: %s,", node->name);
      break;
    case AST_CALL:
      snprintf(buf+strlen(buf), sz, "CALL: %s(),", node->name);
      break;
    case AST_BINOP:
      char op[2] = {0};
      switch (node->op) {
        case TOKEN_PLUS:  strcpy(op, "+"); break;
        case TOKEN_MINUS: strcpy(op, "-"); break;
        case TOKEN_MUL:   strcpy(op, "*"); break;
        case TOKEN_DIV:   strcpy(op, "/"); break;
        default: error_exit("format() - unknown op\n");
      }
      snprintf(buf+strlen(buf), sz, "BINOP: %s,", op);
      break;
    case AST_FUNCTION:
      snprintf(buf+strlen(buf), sz, "FUNCTION: %s %s,", type_to_str(node->specs_type), node->name);
      break;
    case AST_RETURN:
      strcpy(buf+strlen(buf), "RETURN,");
      break;
    case AST_DECL:
      snprintf(buf+strlen(buf), sz, "DECL: %s %s,", type_to_str(node->specs_type), node->name);
      break;
    case AST_COMPOUND:
      strcpy(buf+strlen(buf), "COMPOUND,");
      break;
    case AST_ASSIGNMENT:
      if (node->name == NULL) strcpy(buf+strlen(buf), "ASSIGNMENT:,");
      else snprintf(buf+strlen(buf), sz, "ASSIGNMENT: %s,", node->name);
      break;
    default: error_exit("format() - default reached\n");
  }
  return buf;
}

char *format_annot(AST_t *paren, AST_t *child, char *annot)
{
  size_t sz = 1024;
  char *buf = calloc(sz, sizeof(char));
  snprintf(buf, sz, "%s%s->%s\n", format(paren), format(child), annot);
  return buf;
}

// format AST in a way that can be used by plot.py
void make_ast_graph(AST_t *root)
{
  assert(root != NULL && "create_ast_file - root is NULL");
  List_t *list = init_list(sizeof(char *));   // data to write to file
  List_t *queue = init_list(sizeof(AST_t *)); // queue to iterate on
  List_t *annot = init_list(sizeof(char *));  // TODO: build annotations for arrows
  list_push(queue, root);
  while (queue->size) {
    AST_t *node = (AST_t *)list_getitem(queue, 0);
    list_push(list, format(node));
    list_pop_first(queue);
    if (node == NULL) continue;
    switch (node->node_type) {
      // terminals
      case AST_NUM:
      case AST_ID:
      case AST_DECL:
      case AST_CALL:
        break;
      case AST_BINOP:
        list_push(list, format(node->left));
        list_push(annot, format_annot(node, node->left, "left"));
        list_push(list, format(node->right));
        list_push(annot, format_annot(node, node->right, "right"));
        list_push(queue, node->left);
        list_push(queue, node->right);
        break;
      case AST_FUNCTION:
        // params
        for (int i = 0; i < node->children->size; ++i) {
          list_push(list, format(node->children->items[i]));
          list_push(annot, format_annot(node, node->children->items[i], "param"));
          list_push(queue, node->children->items[i]);
        }
        // body
        list_push(list, format(node->body));
        list_push(annot, format_annot(node, node->body, "body"));
        list_push(queue, node->body);
        break;
      case AST_COMPOUND:
        for (int i = 0; i < node->children->size; ++i) {
          list_push(list, format(node->children->items[i]));
          list_push(annot, format_annot(node, node->children->items[i], "child"));
          list_push(queue, node->children->items[i]);
        }
        break;
      case AST_RETURN:
        list_push(list, format(node->value));
        list_push(annot, format_annot(node, node->value, "value"));
        list_push(queue, node->value);
        break;
      case AST_ASSIGNMENT:
        if (node->decl != NULL) {
          list_push(list, format(node->decl));
          list_push(annot, format_annot(node, node->decl, "decl"));
          list_push(queue, node->decl);
        }
        list_push(list, format(node->value));
        list_push(annot, format_annot(node, node->value, "value"));
        list_push(queue, node->value);
        break;
      default: error_exit("create_ast_file() - unknown node type\n");
    }
    list_push(list, "\n");
  }

  char *path = "pyutil/AST_data.txt";
  FILE *fp;
  if ((fp = fopen(path, "w")) == NULL) {
    printf("Error opening %s\n", path);
    exit(1);
  }

  // tree
  for (int i = 0; i < list->size; ++i)
    fwrite(list->items[i], sizeof(char), strlen(list->items[i]), fp);

  // annotations
  char *separator = "Annotations:\n";
  fwrite(separator, sizeof(char), strlen(separator), fp);
  for (int i = 0; i < annot->size; ++i)
    fwrite(annot->items[i], sizeof(char), strlen(annot->items[i]), fp);
  
  fclose(fp);

  // use python script
  system("cd pyutil/ && ./plot.py");
}
