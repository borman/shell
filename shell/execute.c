#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "execute.h"
#include "debug.h"

static int run_sync(char *program, List arguments, int fdin, int fdout, int fderr);
static int replace_process(char *program, List arguments, int fdin, int fdout, int fderr);

static int do_command(CommandNode *node, int fdin, int fdout, int fderr);
static int do_subshell(CommandNode *node, int fdin, int fdout, int fderr);
static int do_chain(CommandNode *node, int fdin, int fdout, int fderr);
static int do_background(CommandNode *node, int fdin, int fdout, int fderr);
static int do_pipe(CommandNode *node, int fdin, int fdout, int fderr);

int execute(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_GREEN "execute ");
  debug_dump_cmdnode(stderr, node);
  fprintf(stderr, "\n" TERM_NORMAL);

  if (node==NULL)
    return 0;

  switch (node->type)
  {
    case CN_COMMAND:
      return do_command(node, fdin, fdout, fderr);

    case CN_PIPE:
      return do_pipe(node, fdin, fdout, fderr);

    case CN_SUBSHELL:
      return do_subshell(node, fdin, fdout, fderr);

    case CN_BACKGROUND:
      return do_command(node, fdin, fdout, fderr);

    case CN_CHAIN:
    case CN_OR:
    case CN_AND:
      return do_chain(node, fdin, fdout, fderr);

    default:
      fprintf(stderr, TERM_FG_BROWN "execute: null node found\n" TERM_NORMAL);
      return 1;
  } 
}

/**
 * Node types
 */

static int do_command(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_BROWN "do_command %s\n" TERM_NORMAL, node->command);
  return run_sync(node->command, node->arguments, fdin, fdout, fderr);
}

static int do_subshell(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_BROWN "do_subshell\n" TERM_NORMAL);
  return execute(node->op1, fdin, fdout, fderr);
}

static int do_chain(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_BROWN "do_chain %s -> ;\n" TERM_NORMAL, node->command);
  execute(node->op1, fdin, fdout, fderr);
  return execute(node->op2, fdin, fdout, fderr);
}

static int do_background(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_BROWN "do_background -> chain\n" TERM_NORMAL);
  execute(node->op1, fdin, fdout, fderr);
  return execute(node->op2, fdin, fdout, fderr);
}

static int do_pipe(CommandNode *node, int fdin, int fdout, int fderr)
{
  fprintf(stderr, TERM_FG_BROWN "do_pipe: oops, my mama doesn't let me pipe :(\n" TERM_NORMAL);
  return 1;
}


/**
 * Utility functions
 */

static int run_sync(char *program, List arguments, int fdin, int fdout, int fderr)
{
  pid_t child;

  fprintf(stderr, TERM_FG_BROWN TERM_BOLD "run command %s ", program);
  debug_dump_szlist(stderr, arguments);
  fprintf(stderr, "\n" TERM_NORMAL);

  if (((child = fork())) == 0)
  {
    /* child process */
    replace_process(program, arguments, fdin, fdout, fderr);
  }
  else
  {
    /* parent process */
    int status;
    pid_t wait_result = waitpid(child, &status, 0);
    if (WIFEXITED(status))
    {
      int retval = WEXITSTATUS(status);
      fprintf(stderr, TERM_FG_BROWN "child exited with code %d\n" TERM_NORMAL,
         retval);
      return retval;
    }
    else if (WIFSIGNALED(status))
    {
      fprintf(stderr, TERM_FG_BROWN "child killed by signal %d\n" TERM_NORMAL,
         WTERMSIG(status));
    }
    else
      fprintf(stderr, TERM_FG_BROWN "child exited with unknown reason\n" TERM_NORMAL);
  }

  return 1;
}

static int replace_process(char *program, List arguments, int fdin, int fdout, int fderr)
{
  size_t argc = list_size(arguments);
  char **argv = (char **) calloc(1 + argc + 1, sizeof(char *));
  size_t i;
  int res;

  argv[0] = program;
  arguments = list_reverse(arguments);
  for (i=1; arguments!=EmptyList; i++, arguments=arguments->next)
    argv[i] = list_head_str(arguments);

  /* TODO: replace in/out/err */

  res = execvp(program, argv);
  free(argv);
  perror("run_sync");
  exit(1);
  /* noreturn */
}
