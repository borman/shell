#include <stdio.h>
#include "debug.h"

void debug_dump_szlist(ListNode *node)
{
  while (node != NULL)
  {
    printf("\"%s\"\n", node->d.c_str==NULL? "<nil>": node->d.c_str);
    node = node->next;
  }
}
