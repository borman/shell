#include <stdio.h>
#include <stdlib.h>

#include "readline.h"

#ifdef USE_GNU_READLINE

#include <readline/readline.h>
#include <readline/history.h>

READLINE_RESULT readline_run(const char *prompt, Buffer *buf)
{
  static int history_ready = 0;
  char *line;
 
  if (!history_ready)
    using_history();

  line = readline(prompt);

  if (line == NULL)
  {
    buffer_clear(buf);
    return READLINE_EOF;
  }

  add_history(line);
  buffer_copy(buf, line);
  free(line);
  return READLINE_NEWLINE;
}

#else

READLINE_RESULT readline_run(const char *prompt, Buffer *buf)
{
  int c;
  
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
