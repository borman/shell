#include "readline.h"
#include "cmdline.h"
#include "debug.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  Buffer *linebuffer = buffer_alloc();
  READLINE_RESULT result;
  
  do
  {
    CommandTree *tree;

    result = readline("borsh$", linebuffer); 
    tree = cmdline_lex(linebuffer->c_str);

    debug_dump_szlist(tree->tokens.root);

    cmdline_free(tree); 

  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

