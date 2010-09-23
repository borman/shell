#include "readline.h"
#include "cmdline.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  Buffer *linebuffer = buffer_alloc();
  READLINE_RESULT result;
  char *prompt;

  if (argc>1 && !strcmp(argv[1], "-q"))
    prompt = NULL;
  else
    prompt = "model_shell$";
  
  do
  {
    CommandTree *tree;

    result = readline(prompt, linebuffer); 
    tree = cmdline_parse(linebuffer->c_str);

    switch (tree->status)
    {
      case CMDLINE_OK:
        debug_dump_szlist(tree->tokens.root);
        break;

      case CMDLINE_LEX_UNBALANCED_QUOTE:
        printf("Syntax error: Unbalanced quotes\n");
        break;
    }

    cmdline_free(tree); 

  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

