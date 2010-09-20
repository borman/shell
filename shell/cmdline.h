#ifndef CMDLINE_H
#define CMDLINE_H

#include "cmdnode.h"

typedef struct 
{
  /* public */
  CommandNode *root;

  /* private */
  char *stringdata;
} CommandTree;

CommandTree *cmdline_lex(const char *cmdline);

int cmdline_exec(CommandNode *node);

#endif /* CMDLINE_H */

