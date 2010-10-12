#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#include "list.h"
#include "cmdnode.h"
#include "colors.h"

/**
 * Print contents of a string list as a column 
 */
void debug_dump_szlist(FILE *file, List list);
void debug_dump_cmdnode(FILE *file, CommandNode *node);
void debug_dump_expression_graph(CommandNode *expr);

#endif /* DEBUG_H */
