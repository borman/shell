#include "cmdnode.h"
#include <stdlib.h>

CommandNode *cmdnode_alloc(COMMANDNODE_TYPE type)
{
  CommandNode *node = (CommandNode *)malloc(sizeof(CommandNode));

  node->type = type;
  node->str = NULL;
  node->next = NULL;

  return node;
}

void cmdnode_free(CommandNode *node)
{
  free(node);
}


