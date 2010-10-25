#ifndef EXECUTE_H
#define EXECUTE_H

#include "cmdnode.h"

/**
 * Execute a command tree.
 *
 * If top-level node is a subshell, it has a special behaviour:
 * no fork is issued and IO redirection is ignored
 */
int execute(CommandNode *node);

#endif /* EXECUTE_H */
