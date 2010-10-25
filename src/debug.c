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


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

void debug_dump_szlist(FILE *file, List list)
{
#ifndef NDEBUG
  fprintf(file, "[");
  while (list != NULL)
  {
    fprintf(file, "'%s'%s", list_head_str(list), list->next==NULL?"":", ");
    list = list->next;
  }
  fprintf(file, "]");
#endif
}

void debug_dump_cmdnode(FILE *file, CommandNode *node)
{
#ifndef NDEBUG
  List list;
  if (node==NULL)
  {
    fprintf(file, "null");
    return;
  }
  switch (node->type)
  {
    case CN_COMMAND:
      fprintf(file, "{command, '%s\', ", node->command);
      debug_dump_szlist(file, node->arguments);
      fprintf(file, "}");
      break;

    case CN_PIPE:
    case CN_CHAIN:
    case CN_AND:
    case CN_OR:
    case CN_BACKGROUND:
      fprintf(file, "{operator, '%s', ", node->command);
      debug_dump_cmdnode(file, node->op1);
      fprintf(file, ", ");
      debug_dump_cmdnode(file, node->op2);
      fprintf(file, "}");
      break;

    case CN_SUBSHELL:
      fprintf(file, "{subshell, ");
      list = node->expression;
      if (node->op1!=NULL)
      {
        debug_dump_cmdnode(file, node->op1);
      }
      else
      {
        fprintf(file, "[");
        while (list != NULL)
        {
          debug_dump_cmdnode(file, list_head_command(list));
          fprintf(file, "%s", list->next==NULL?"":", ");
          list = list->next;
        }
        fprintf(file, "]");
      }
      fprintf(file, "}");
      break;

    default:
      fprintf(file, "unknown");
  }
#endif
}

static void debug_dump_expression_graph_node(FILE *file, CommandNode *expr);

void debug_dump_expression_graph(CommandNode *expr)
{
#ifndef NDEBUG
  FILE *file = fopen("/tmp/shell-command.dot", "w");
  if (file == NULL)
    return;
  fprintf(file, "digraph Cmdline {\n");
  debug_dump_expression_graph_node(file, expr);
  fprintf(file, "}\n");
  fclose(file);
#endif
}

static void debug_dump_expression_graph_node(FILE *file, CommandNode *expr)
{
  switch (expr->type)
  {
    case CN_SUBSHELL:
      fprintf(file, "node%p [shape=box, label=\"Subshell\"];\n", (void *)expr);
      break;
  
    case CN_COMMAND:
      fprintf(file, "node%p [shape=ellipse, label=\"%s\\n", (void *)expr, expr->command);
      debug_dump_szlist(file, expr->arguments);
      fprintf(file, "\"];\n");
      break;

    default:
      fprintf(file, "node%p [shape=diamond, label=\"%s\"]\n", (void *)expr, expr->command);
  }
  if (expr->op1 != NULL)
  {
    fprintf(file, "node%p -> node%p;\n", (void *)expr, (void *)expr->op1);
    debug_dump_expression_graph_node(file, expr->op1);
  }
  if (expr->op2 != NULL)
  {
    fprintf(file, "node%p -> node%p;\n", (void *)expr, (void *)expr->op2);
    debug_dump_expression_graph_node(file, expr->op2);
  }
}

void trace(const char *format, ...)
{
#if (!defined(NDEBUG)) && (!defined(NOTRACE))
  va_list args;
  va_start(args, format);
  fprintf(stderr, TERM_FG_CYAN TERM_BOLD "[Trace] " TERM_NORMAL TERM_FG_CYAN);
  vfprintf(stderr, format, args);
  fprintf(stderr, TERM_NORMAL "\n");
  va_end(args);
#endif
}
