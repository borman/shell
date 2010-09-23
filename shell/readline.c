#include "readline.h"
#include <stdio.h>

READLINE_RESULT readline(const char *prompt, Buffer *buf)
{
  char c;
  
  buffer_clear(buf);

  if (prompt != NULL)
    printf("%s ", prompt);
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
