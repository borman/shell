#ifndef CMDNODE_H
#define CMDNODE_H

typedef enum
{
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

#endif /* CMDNODE_H */

