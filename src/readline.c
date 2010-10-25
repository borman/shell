#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "readline.h"

#ifdef USE_GNU_READLINE

#include <readline/readline.h>
#include <readline/history.h>

static READLINE_RESULT readline_run_gnu_readline(const char *prompt, Buffer *buf)
{
  static int history_ready = 0;
  char *line;
 
  if (!history_ready)
  {
    using_history();
    history_ready = 1;
  }

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

#endif

static READLINE_RESULT readline_run_simple(const char *prompt, Buffer *buf)
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

READLINE_RESULT readline_run(const char *prompt, Buffer *buf)
{
  if (isatty(0))
  {
#ifdef USE_GNU_READLINE
    return readline_run_gnu_readline(prompt, buf);
#else
    return readline_run_simple(prompt, buf);
#endif
  }
  else
    return readline_run_simple(NULL, buf);
}

