#include <strings.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "cmdline.h"
#include "cmdline_lexer.h"
#include "cmdline_parser.h"
#include "cmdnode.h"
#include "debug.h"

/* Make a list out of a chain of strings. */
static List make_szlist(Buffer *strings)
{
  size_t pos = 0;
  List list = EmptyList;

  while (pos<strings->length)
  {
    size_t delta = strlen(strings->c_str+pos);

    list = list_push(list, strings->c_str + pos);
    pos += delta+1;
  }

  return list;
}

Program *cmdline_parse(const char *cmdline)
{
  Program *prog = (Program *)calloc(1, sizeof(Program));

  prog->strings = lexer_split(cmdline, &prog->status);
  if (prog->status != CMDLINE_OK)
      return prog;

  prog->tokens = list_reverse(make_szlist(prog->strings));
  debug_dump_szlist(stderr, prog->tokens);
  fprintf(stderr, "\n");

  prog->tree = parser_buildtree(prog->tokens, &prog->status);
  debug_dump_cmdnode(stderr, prog->tree);
  fprintf(stderr, "\n");

  if (prog->status != CMDLINE_OK)
      return prog;

  /* TODO: Split all flat subexpressions into subtrees */
  cmdnode_unflatten(prog->tree, &prog->status);

  return prog;
}

void cmdline_free(Program *prog)
{
  if (prog->strings != NULL)
    buffer_free(prog->strings);
  if (prog->tokens != NULL)
    list_free(prog->tokens);
  if (prog->tree != NULL)
    cmdnode_free_recursive(prog->tree);

  free(prog);
}

