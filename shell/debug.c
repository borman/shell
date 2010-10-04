#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

void debug_dump_szlist(List list)
{
  fprintf(stderr, "[ ");
  while (list != NULL)
  {
    fprintf(stderr, "\"%s\", ", list_head_str(list));
    list = list->next;
  }
  fprintf(stderr, "]\n");
}

