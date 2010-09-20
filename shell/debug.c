#include "debug.h"
#include <stdio.h>

void debug_dump_cmdnode_list(CommandNode *node)
{
  while (node != NULL)
  {
    printf("%s\n", node->str==NULL? "<nil>": node->str);
    node = node->next;
  }
}
