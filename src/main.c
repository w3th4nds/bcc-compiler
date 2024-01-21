#include "include/bcc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if (argc < 2) {
    puts("[-] Specify source file.");
    return 1;
  }
  bcc_compile_file(argv[1]);
  return 0;
}
