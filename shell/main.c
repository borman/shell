#include "readline.h"
#include "cmdline.h"
#include "debug.h"
#include "colors.h"
#include <stdio.h>
#include <string.h>

static void print_error(const char *error_class, const char *error_desc)
{
  fprintf(stderr, TERM_RED "%s" TERM_NORMAL ": %s\n", error_class, error_desc);
}

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
    prompt = TERM_GREEN "model_shell> " TERM_NORMAL;
  
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
        print_error("Syntax error", "Unbalanced quote");
        break;

      case CMDLINE_LEX_UNFINISHED_ESCAPE:
        print_error("Syntax error", "Unfinished escape-sequence");
        break;

      case CMDLINE_PARSER_ERROR:
        printf("Unknown syntax error\n");
        break;
    }

    cmdline_free(prog); 

  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

