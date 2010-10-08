#ifndef CMDLINE_H
#define CMDLINE_H

#include "list.h"
#include "buffer.h"
#include "cmdnode.h"
#include "cmdline_status.h"

typedef struct 
{
  /* public */
  CmdlineParserStatus status; /* error code */
  List tokens; /* Lexer output */

  /* private */
  Buffer *strings;
  CommandNode *tree;
} Program;

/** 
 * Perform lexical parsing of cmdline.
 * 
 * Returns a non-NULL pointer in any case.
 *
 * TODO: Only lexing is performed now.
 * result.tokens is set to a list of tokens.
 * result.status indicates if there were errors.
 */
Program *cmdline_parse(const char *cmdline);
/**
 * Free resources allocated for parsed command line string.
 */
void cmdline_free(Program *tree);

#endif /* CMDLINE_H */

