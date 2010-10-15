#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include "cmdline.h"
#include "cmdnode.h"

CommandNode *parser_buildtree(List tokens, Diagnostic *diag);
  
#endif /* CMDLINE_PARSER_H */

