#ifndef CMDNODE_H
#define CMDNODE_H

/**
 * Command line program is represented by a binary tree
 */

/**
 * Node class
 */
typedef enum
{
  CN_NULL,
  CN_COMMAND, 
  CN_CHAIN, 
  CN_AND,
  CN_OR,
  CN_PIPE,
  CN_SUBSHELL,
  CN_BACKGROUND
} CommandNodeType;

typedef struct CommandNodeRef 
{
  CommandNodeType type;

  char *command;

  int argc;
  char **argv;

  char *input;
  char *output;
  char *append_output;

  struct CommandNodeRef *op1;
  struct CommandNodeRef *op2;
} CommandNode;


CommandNode *cmdnode_alloc();
void cmdnode_free(CommandNode *node);
void cmdnode_free_recursive(CommandNode *root);

#endif /* CMDNODE_H */

