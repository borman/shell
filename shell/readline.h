#ifndef READLINE_H
#define READLINE_H

#include "buffer.h"

/* Prompt result */
typedef enum
{
  READLINE_NEWLINE,
  READLINE_EOF
} READLINE_RESULT;

/**
 * Print a prompt and read input up to end of line or
 * end of file.
 *
 * Result is stored in buffer.
 * Buffer MUST be pre-allocated. Buffer contents will be erased.
 *
 * If prompt is NULL, it is not printed.
 */
READLINE_RESULT readline(const char *prompt, Buffer *buf);

#endif /* READLINE_H */

