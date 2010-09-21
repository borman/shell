#ifndef CMDLINE_H
#define CMDLINE_H

#include "szlist.h"
#include "buffer.h"

typedef enum 
{
  CMDLINE_OK = 0,
  CMDLINE_LEX_UNBALANCED_QUOTE
} CMDLINE_PARSER_STATUS;

typedef struct 
{
  /* public */
  /* Lexer output */
  SzList tokens;
  CMDLINE_PARSER_STATUS status;

  /* private */
  Buffer *stringdata;
} CommandTree;

CommandTree *cmdline_lex(const char *cmdline);
void cmdline_parse(CommandTree *tree);
void cmdline_free(CommandTree *tree);

#endif /* CMDLINE_H */

