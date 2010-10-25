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
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "readline.h"
#include "cmdline.h"
#include "debug.h"
#include "colors.h"
#include "execute.h"

static void print_error(const char *error_class, const char *error_desc);
static void collect_zombies();

int main(int argc, char **argv)
{
  Buffer *linebuffer = buffer_alloc();
  READLINE_RESULT result;
  char *prompt;

  prompt = TERM_FG_GREEN TERM_BOLD "model_shell> " TERM_NORMAL;
  
  do
  {
    Program *prog;

    result = readline_run(prompt, linebuffer); 
    prog = cmdline_parse(linebuffer->c_str);

    if (prog->diag.error)
    {
      print_error("Error", prog->diag.error_message);
    }
    else
    {
      debug_dump_expression_graph(prog->tree);
      execute(prog->tree);
    }

    cmdline_free(prog); 

    collect_zombies();
  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

/**
 * Print an error message with colors and formatting 
 */
static void print_error(const char *error_class, const char *error_desc)
{
  fprintf(stderr, TERM_FG_RED TERM_BOLD "%s:" TERM_NORMAL " %s\n", error_class, error_desc);
}

/**
 * Fetch exit statuses for finished background processes.
 *
 * FIXME: non-direct background child processes are not handled
 */
static void collect_zombies()
{
  pid_t pid;
  int status;
  while (((pid = waitpid(-1, &status, WNOHANG))) != 0)
  {
    if (pid == -1)
    {
      if (errno == EINTR)
        continue;
      else
        break;
    }
    /* pid is valid */
    fprintf(stderr, "BG process %d terminated: ", pid);
    
    if (WIFEXITED(status))
      fprintf(stderr, "exited with code %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
      fprintf(stderr, "killed by signal %d\n", WTERMSIG(status));
    else
      fprintf(stderr, "unknown reason\n");
  }  
}
