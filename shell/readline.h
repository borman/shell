#ifndef READLINE_H
#define READLINE_H

#include "buffer.h"

typedef enum
{
  READLINE_NEWLINE,
  READLINE_EOF
} READLINE_RESULT;


READLINE_RESULT readline(Buffer *buf);

#endif /* READLINE_H */

