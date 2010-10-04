#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmdnode.h"

static const CommandNode empty_node =
{
  .type = CN_NULL,
  .command = NULL,
  .argv = NULL,
  .input_files = {NULL, NULL},
  .output_files = {NULL, NULL},
  .output_append_files = {NULL, NULL},
  .expression = NULL,
  .op1 = NULL,
  .op2 = NULL
};

CommandNode *cmdnode_alloc(CommandNodeType type)
{
  CommandNode *node = (CommandNode *) malloc(sizeof(CommandNode));
  memcpy(node, &empty_node, sizeof(CommandNode));
  node->type = type;

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

