#include "debug.h"
#include <stdio.h>

void debug_dump_szlist(SzListNode *node)
{
  while (node != NULL)
  {
    printf("\"%s\"\n", node->c_str==NULL? "<nil>": node->c_str);
    node = node->next;
  }
}
