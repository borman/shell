/*
 * Copyright 2010, Mikhail "borman" Borisov <borisov.mikhail@gmail.com>
 *
 * This file is part of borman's model shell.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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

