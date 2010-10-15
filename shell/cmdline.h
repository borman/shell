#ifndef CMDLINE_H
#define CMDLINE_H

#include "list.h"
#include "buffer.h"
#include "cmdnode.h"
#include "diagnostic.h"

typedef struct 
{
  Diagnostic diag;   /* Error flag and description */

  List tokens;       /* Lexer output */
  CommandNode *tree; /* Execution tree */

  Buffer *strings;   /* Token storage */
} Program;

/** 
 * Perform lexical parsing of cmdline.
 * 
 * Returns a non-NULL pointer in any case.
 */
Program *cmdline_parse(const char *cmdline);

/**
 * Free resources allocated for parsed command line string.
 */
void cmdline_free(Program *tree);

#endif /* CMDLINE_H */

