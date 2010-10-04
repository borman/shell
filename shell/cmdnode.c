#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmdnode.h"

static const struct operator_definition
{
  const char *operator;
  CommandNodeType type;
} operator_definitions[] = 
{
  {"|",  CN_PIPE},
  {";",  CN_CHAIN},
  {"&",  CN_BACKGROUND},
  {"&&", CN_AND},
  {"||", CN_OR}
};
static const size_t n_operator_definitions = 
  sizeof(operator_definitions)/sizeof(struct operator_definition);

static CommandNodeType operator_type(const char *operator)
{
  size_t i;
  for (i=0; i<n_operator_definitions; i++)
    if (!strcmp(operator_definitions[i].operator, operator))
      return operator_definitions[i].type;

  return CN_NULL;
}

static CommandNode *cmdnode_alloc(CommandNodeType type, char *command)
{
  CommandNode *node = (CommandNode *) calloc(1, sizeof(CommandNode));
  node->type = type;
  node->command = command;

  return node; 
}

CommandNode *cmdnode_command(char *command)
{
  return cmdnode_alloc(CN_COMMAND, command);
}

CommandNode *cmdnode_operator(char *operator)
{
  return cmdnode_alloc(operator_type(operator), operator);
}

CommandNode *cmdnode_subshell(List expression)
{
  CommandNode *node = cmdnode_alloc(CN_SUBSHELL, NULL);
  node->expression = expression;

  return node;
}


void cmdnode_add_redirection(CommandNode *node, char *type, char *filename)
{
  if (!strcmp("<", type))
    node->input_files = list_push(node->input_files, filename);
  else if (!strcmp(">", type))
    node->output_files = list_push(node->output_files, filename);
  else if (!strcmp(">>", type))
    node->output_append_files = list_push(node->output_append_files, filename);
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

