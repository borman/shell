#ifndef CMDNODE_H
#define CMDNODE_H

typedef enum
{
  COMMANDNODE_NULL,
  COMMANDNODE_WORD,
  COMMANDNODE_PROGRAM,
  COMMANDNODE_ARGUMENT
} COMMANDNODE_TYPE;

typedef struct CommandNodeRef 
{
  COMMANDNODE_TYPE type;
  char *str;
  struct CommandNodeRef *next;
} CommandNode;

CommandNode *cmdnode_alloc(COMMANDNODE_TYPE type);
void cmdnode_free(CommandNode *node);

#endif /* CMDNODE_H */

