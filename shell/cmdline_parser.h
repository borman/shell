#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include "cmdline.h"
#include "cmdnode.h"

CommandNode *parser_buildtree(List tokens, CmdlineParserStatus *status);
  
#endif /* CMDLINE_PARSER_H */

