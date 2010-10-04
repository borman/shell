#include <strings.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "cmdline.h"
#include "cmdline_lexer.h"
#include "cmdline_parser.h"

/* Make a list out of a chain of strings.
 * *list is considered uninitialized */
static void make_szlist(List *list, const Buffer *strings)
{
  size_t pos = 0;

  list_init(list, NULL);
  while (pos<strings->length)
  {
    size_t delta = strlen(strings->c_str+pos);

    list_append(list, list_node_alloc(strings->c_str+pos));
    pos += delta+1;
  }
}

Program *cmdline_parse(const char *cmdline)
{
  Program *prog = (Program *)malloc(sizeof(Program));
  CommandNode *tree;

  prog->strings = buffer_alloc();

  prog->status = lexer_split(prog->strings, cmdline);
  make_szlist(&prog->tokens, prog->strings);

  parser_buildtree(&tree, prog->tokens.root);

  return prog;
}

void cmdline_free(Program *prog)
{
  buffer_free(prog->strings);
  list_destroy(&prog->tokens);

  free(prog);
}

