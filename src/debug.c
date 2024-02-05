#include "include/debug.h"

int SHOW_SOURCE = 1; // will be changed to 0 when everything is working
int GRAPH = 0; // create graph of the parsed AST
int LEX_DEBUG = 0;
int PARSE_DEBUG = 0;
int SCOPE_DEBUG = 0;
int ASM_DEBUG = 0;

void init_debug(void)
{
  char *show_src = getenv("SHOW_SOURCE");
  if (show_src != NULL && strcmp(show_src, "0") == 0) SHOW_SOURCE = 0;

  char *plot_expr = getenv("GRAPH");
  if (plot_expr != NULL && strcmp(plot_expr, "1") == 0) GRAPH = 1;

  char *debug = getenv("DEBUG");
  if (debug == NULL) return;
  // FULL section debug
  if (strcmp(debug, "FULL") == 0) {
    PARSE_DEBUG = 1;
    SCOPE_DEBUG = 1;
    ASM_DEBUG = 1;
  }
  else {
    char *tok = strtok(debug, ",");
    while (tok != NULL) {
      if (strcmp(tok, "LEX") == 0)        LEX_DEBUG = 1;
      else if (strcmp(tok, "PARSE") == 0) PARSE_DEBUG = 1;
      else if (strcmp(tok, "SCOPE") == 0) SCOPE_DEBUG = 1;
      else if (strcmp(tok, "ASM") == 0)   ASM_DEBUG = 1;
      tok = strtok(NULL, ",");
    }
  }

  if (LEX_DEBUG | PARSE_DEBUG | ASM_DEBUG) {
    printf("DEBUGGING:\n"
           "    LEX = %d\n"
           "  PARSE = %d\n"
           "  SCOPE = %d\n"
           "    ASM = %d\n\n",
           LEX_DEBUG, PARSE_DEBUG, SCOPE_DEBUG, ASM_DEBUG);
  }
}

void error_exit(char *msg)
{
  printf("[!] ERROR: %s\n"
         "[ FAILURE ]\n", msg);
  exit(1);
}
