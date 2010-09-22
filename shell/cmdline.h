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
  Buffer *strings;
} CommandTree;

/** 
 * Perform lexical parsing of cmdline.
 * 
 * Returns a non-NULL pointer in any case.
 * result.tokens is set to a list of tokens.
 * result.status indicates if there were errors.
 */
CommandTree *cmdline_lex(const char *cmdline);
/**
 * Free resources allocated for parsed command line string.
 */
void cmdline_free(CommandTree *tree);

#endif /* CMDLINE_H */

