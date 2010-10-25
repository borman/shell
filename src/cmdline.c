/*
 * Copyright 2010, Mikhail "borman" Borisov <borisov.mikhail@gmail.com>
 *
 * This file is part of borman's model shell.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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
static List make_token_list(Buffer *strings)
{
  size_t pos = 0;
  List list = NULL;

  while (pos<strings->length)
  {
    size_t delta = strlen(strings->c_str+pos);

    list = list_push(list, strings->c_str + pos);
    pos += delta+1;
  }

  return list_reverse(list);
}

Program *cmdline_parse(const char *cmdline)
{
  Program *prog = (Program *) calloc(1, sizeof(Program));

  prog->strings = lexer_split(cmdline, &prog->diag);
  if (prog->diag.error)
    return prog;

  prog->tokens = make_token_list(prog->strings);

  /* Dump tokens list */
  /*
  debug_dump_szlist(stderr, prog->tokens);
  fprintf(stderr, "\n");
  */

  prog->tree = parser_buildtree(prog->tokens, &prog->diag);
  if (prog->diag.error)
    return prog;

  /* Dump command tree */
  /*
  debug_dump_cmdnode(stderr, prog->tree);
  fprintf(stderr, "\n");
  */

  cmdnode_unflatten(prog->tree, &prog->diag);

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

