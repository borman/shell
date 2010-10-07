#ifndef CMDNODE_H
#define CMDNODE_H

#include "list.h"

/**
 * Command line program is represented by a binary tree
 */

/**
 * Node class
 */
typedef enum
{
  CN_NULL       = 0x0,
  CN_COMMAND    = 0x1, 
  CN_CHAIN      = 0x2, 
  CN_AND        = 0x4,
  CN_OR         = 0x8,
  CN_PIPE       = 0x10,
  CN_SUBSHELL   = 0x20,
  CN_BACKGROUND = 0x40
} CommandNodeType;

typedef struct CommandNodeRef 
{
  CommandNodeType type;

  char *command;

  List arguments;

  List input_files;
  List output_files;
  List output_append_files;

  List expression;

  struct CommandNodeRef *op1;
  struct CommandNodeRef *op2;
} CommandNode;


CommandNode *cmdnode_command(char *command);
CommandNode *cmdnode_operator(char *operator);
CommandNode *cmdnode_subshell(List expression);

void cmdnode_add_redirection(CommandNode *node, char *type, char *filename);

void cmdnode_free(CommandNode *node);
void cmdnode_free_recursive(CommandNode *root);

void cmdnode_unflatten(CommandNode *node, int *status);
#endif /* CMDNODE_H */

