#ifndef CMDLINE_LEXER_H
#define CMDLINE_LEXER_H

#include "cmdline.h"

Buffer *lexer_split(const char *src, Diagnostic *diag);

#endif /* CMDLINE_LEXER_H */

