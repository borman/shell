#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

void debug_dump_szlist(FILE *file, List list)
{
  fprintf(file, "[");
  while (list != NULL)
  {
    fprintf(file, "\"%s\"%s", list_head_str(list), list->next==NULL?"":", ");
    list = list->next;
  }
  fprintf(file, "]");
}

void debug_dump_cmdnode(FILE *file, CommandNode *node)
{
  List list;
  if (node==NULL)
  {
    fprintf(file, "null");
    return;
  }
  switch (node->type)
  {
    case CN_COMMAND:
      fprintf(file, "{command, \"%s\", ", node->command);
      debug_dump_szlist(file, node->arguments);
      fprintf(file, "}");
      break;

    case CN_PIPE:
    case CN_CHAIN:
    case CN_AND:
    case CN_OR:
    case CN_BACKGROUND:
      fprintf(file, "{operator, \"%s\", ", node->command);
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
}

