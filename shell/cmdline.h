#ifndef CMDLINE_H
#define CMDLINE_H

#include "cmdnode.h"
#include "buffer.h"

typedef struct 
{
  /* public */
  CommandNode *root;
  int errno;

  /* private */
  Buffer *stringdata;
} CommandTree;

CommandTree *cmdline_lex(const char *cmdline);
void cmdline_parse(CommandTree *tree);
void cmdline_free(CommandTree *tree);

int cmdline_exec(CommandNode *node);

#endif /* CMDLINE_H */

