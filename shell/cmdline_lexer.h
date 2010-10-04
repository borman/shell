#ifndef CMDLINE_LEXER_H
#define CMDLINE_LEXER_H

#include "cmdline.h"

Buffer *lexer_split(const char *src, CmdlineParserStatus *result);

#endif /* CMDLINE_LEXER_H */

