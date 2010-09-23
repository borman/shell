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

  /*
   * -q argument to omit printing prompt
   */
  if (argc>1 && !strcmp(argv[1], "-q"))
    prompt = NULL;
  else
    prompt = "model_shell> ";
  
  do
  {
    Program *prog;

    result = readline_run(prompt, linebuffer); 
    prog = cmdline_parse(linebuffer->c_str);

    switch (prog->status)
    {
      case CMDLINE_OK:
        debug_dump_szlist(prog->tokens.root);
        break;

      case CMDLINE_LEX_UNBALANCED_QUOTE:
        printf("Syntax error: Unbalanced quotes\n");
        break;

      case CMDLINE_LEX_UNFINISHED_ESCAPE:
        printf("Syntax error: Unfinished escape-sequence\n");
        break;
    }

    cmdline_free(prog); 

  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

