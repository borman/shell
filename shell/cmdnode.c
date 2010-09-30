#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmdnode.h"

static const CommandNode empty_node =
{
  .type = CN_NULL,
  .command = NULL,
  .argc = 0,
  .argv = NULL,
  .input = NULL,
  .output = NULL,
  .append_output = NULL,
  .op1 = NULL,
  .op2 = NULL
};

CommandNode *cmdnode_alloc()
{
  CommandNode *node = (CommandNode *) malloc(sizeof(CommandNode));
  memcpy(node, &empty_node, sizeof(CommandNode));

  return node; 
}

void cmdnode_free(CommandNode *node)
{
  free(node);
}

void cmdnode_free_recursive(CommandNode *root)
{
  if (root->op1 != NULL)
    cmdnode_free_recursive(root->op1);

  if (root->op2 != NULL)
    cmdnode_free_recursive(root->op2);

  if (root->argv != NULL)
    free(root->argv);

  free(root);
}

