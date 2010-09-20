#include "readline.h"
#include <stdio.h>

READLINE_RESULT readline(Buffer *buf)
{
  char c;
  printf("$ ");
  while (1) 
  {
    c = getchar();
    if (c==EOF)
      return READLINE_EOF;
    if (c=='\n')
      return READLINE_NEWLINE;
  }
}
