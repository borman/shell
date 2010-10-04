#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include "cmdline.h"
#include "cmdnode.h"

CmdlineParserStatus parser_buildtree(CommandNode **root, ListNode *tokens);
  
#endif /* CMDLINE_PARSER_H */

