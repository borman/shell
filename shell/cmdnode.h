#ifndef CMDNODE_H
#define CMDNODE_H

#include "list.h"
#include "cmdline_status.h"

/**
 * Command line program is represented by a binary tree.
 *
 * Each node is either an operator, or a command (leaf node).
 * A program is executed by recursing into the tree.
 */

/**
 * Node class
 * 
 * Classes form a bit mask to be able to specify sets of classes
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

/**
 * A node in internal program representation.
 */
typedef struct CommandNode 
{
  CommandNodeType type;

  char *command;

  List arguments;

  List input_files;
  List output_files;
  List output_append_files;

  List expression; 

  struct CommandNode *op1;
  struct CommandNode *op2;
} CommandNode;

/** 
 * CommandNode allocation frontends 
 */
CommandNode *cmdnode_command(char *command);
CommandNode *cmdnode_operator(char *operator);
CommandNode *cmdnode_subshell(List expression);

/**
 * Add stream redirection to a command node
 * type is one of ">", "<", ">>"
 */
void cmdnode_add_redirection(CommandNode *node, char *type, char *filename);

void cmdnode_free(CommandNode *node);
void cmdnode_free_recursive(CommandNode *root);

/** 
 * Transform a CommandNode tree so that for each subshell node
 * a list of its command is folded into a CommandNode tree.
 */
void cmdnode_unflatten(CommandNode *node, CmdlineParserStatus *status);
#endif /* CMDNODE_H */

