#include <stdio.h>
#include <stdlib.h>

#include "readline.h"

#ifdef USE_GNU_READLINE

#include <readline/readline.h>

READLINE_RESULT readline_run(const char *prompt, Buffer *buf)
{
  char *line = readline(prompt);

  if (line == NULL)
  {
    buffer_clear(buf);
    return READLINE_EOF;
  }

  buffer_copy(buf, line);
  free(line);
  return READLINE_NEWLINE;
}

#else

READLINE_RESULT readline_run(const char *prompt, Buffer *buf)
{
  char c;
  
  buffer_clear(buf);

  if (prompt != NULL)
    printf("%s", prompt);
  while (1) 
  {
    c = getchar();

    if (c==EOF)
      return READLINE_EOF;
    if (c=='\n')
      return READLINE_NEWLINE;

    buffer_putchar(buf, c);
  }
}

#endif
