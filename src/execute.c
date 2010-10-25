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


#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "execute.h"
#include "debug.h"

static int replace_process(char *program, List arguments);
static void redirect_stream(int fd_source, int fd_target);
static void redirect_to_files(const char *input, const char *output, int do_output_append);
static void setup_redirections(CommandNode *node);
static int check_wait(pid_t pid);
static int builtin_cd(List argv);

static int do_execute(CommandNode *node);
static int do_command(CommandNode *node);
static int do_subshell(CommandNode *node);
static int do_chain(CommandNode *node);
static int do_background(CommandNode *node);
static int do_pipe(CommandNode *node);

int execute(CommandNode *node)
{
  if (node==NULL)
    return 0;
  else if (node->type==CN_SUBSHELL)
    return do_execute(node->op1);
  else
    return do_execute(node);
}

/**
 * Perform actual tree execution
 */
static int do_execute(CommandNode *node)
{
  if (node==NULL)
    return 0;

  switch (node->type)
  {
    case CN_COMMAND:
      return do_command(node);

    case CN_PIPE:
      return do_pipe(node);

    case CN_SUBSHELL:
      return do_subshell(node);

    case CN_BACKGROUND:
      return do_background(node);

    case CN_CHAIN:
    case CN_OR:
    case CN_AND:
      return do_chain(node);

    default:
      assert(0);
      return 1;
  } 
}

/**
 * Node types
 */

/**
 * Command node: fork synchronously and call exec
 */
static int do_command(CommandNode *node)
{
  pid_t child;

  if (strcmp(node->command, "cd") == 0)
    return builtin_cd(node->arguments);

  if (((child = fork())) == 0)
  {
    /* child process */
    setup_redirections(node);
    replace_process(node->command, node->arguments);
    /* noreturn */
  }
  else
  {
    /* parent process */
    return check_wait(child);
  }
  return 1;
}

/**
 * Subshell node: fork synchronously and setup environment for subshell 
 */
static int do_subshell(CommandNode *node)
{
  pid_t pid;

  if (((pid = fork())) == 0)
  {
    /* child */
    setup_redirections(node);
    exit(do_execute(node->op1));
    /* noreturn */
  }
  else if (pid > 0)
  {
    /* parent */
    return check_wait(pid);
  }
  /* else */ 
  perror("do_subshell");
  exit(1);
}

/**
 * Chain node: execute left operand and decide whether to run
 * the right one based on left one's return value
 */
static int do_chain(CommandNode *node)
{
  int retval;
  int exec_second;

  retval = do_execute(node->op1);
  switch (node->type)
  {
    case CN_CHAIN:
      exec_second = 1;
      break;
    case CN_AND:
      exec_second = !retval;
      break;
    case CN_OR:
      exec_second = retval;
      break;

    default: /* this isn't going to happen */
      assert(0);
  }
  if (exec_second)
    return do_execute(node->op2);
  else
    return retval;
}

/**
 * Background node: fork and execute left operand in background, while 
 * continuing to do_execute right operand
 */
static int do_background(CommandNode *node)
{
  pid_t pid;

  if (((pid=fork()))==0)
  {
    /* child */
    exit(do_execute(node->op1));
  }
  else if (pid>0)
  {
    trace("Spawned bg process %d", pid);
    return do_execute(node->op2);
  }
  else
  {
    perror("do_background");
    exit(1);
  }
}

/**
 * Pipe: TODO
 */
static int do_pipe(CommandNode *node)
{
  int pipefd[2];

  if (pipe(pipefd) == 0)
  {
    int readfd = pipefd[0];
    int writefd = pipefd[1];

    pid_t source_pid;
    pid_t target_pid;

    if (((source_pid = fork())) == 0)
    {
      /* source child */
      int retval;
      close(readfd);
      redirect_stream(writefd, STDOUT_FILENO);
      retval = execute(node->op1);
      close(writefd);
      exit(retval);
    }
    else
    {
      /* parent */
      close(writefd);
      if (((target_pid = fork())) == 0)
      {
        /* target child */
        int retval;
        redirect_stream(readfd, STDIN_FILENO);
        retval = execute(node->op2);
        close(readfd);
        exit(retval);
      }
      else
      {
        /* parent */
        close(readfd);
        check_wait(source_pid);
        return check_wait(target_pid);
      }
    }
  }
  else
  {
    perror("do_pipe");
    return 1;
  }
}


/**
 * Utility functions
 */

/**
 * Replace current process with a program 
 */
static int replace_process(char *program, List arguments)
{
  size_t argc = list_size(arguments);
  char **argv = (char **) calloc(1 + argc + 1, sizeof(char *));
  size_t i;

  /* build argv array */
  argv[0] = program;
  for (i=1; arguments!=NULL; i++, arguments=arguments->next)
    argv[i] = list_head_str(arguments);

  execvp(program, argv);

  perror(program);
  free(argv);
  exit(1);
  /* noreturn */
}

/**
 * Load redirections from a command node
 */
static void setup_redirections(CommandNode *node)
{
  size_t n_input = list_size(node->input_files);
  size_t n_output = list_size(node->output_files);
  size_t n_output_append = list_size(node->output_append_files);

  const char *output = NULL;
  const char *input = NULL;
  int do_output_append = 0;

  if (n_input>0)
  {
    input = list_head_str(node->input_files);
    if (n_input>1)
      fprintf(stderr, "more than one input file is specified: using last one\n");
  }
  if (n_output>0)
  {
    output = list_head_str(node->output_files);
    if (n_output>1 || n_output_append>0)
      fprintf(stderr, "more than one output file is specified: using last one\n"); 
  }
  else if (n_output_append>0)
  {
    output = list_head_str(node->output_append_files);
    do_output_append = 1;
    if (n_output_append>1)
      fprintf(stderr, "more than one output-append file is specified: using last one\n"); 
  }
  
  redirect_to_files(input, output, do_output_append); 
}

/**
 * Redirect stdin/stdout to corresponding files
 */
static void redirect_to_files(const char *input, const char *output, int do_output_append)
{
  if (input != NULL)
  {
    int fd = open(input, O_RDONLY);
    if (fd < 0)
      perror("Cannot open input file");
    else
      redirect_stream(fd,  STDIN_FILENO);
  }
  if (output != NULL)
  {
    int fd = open(output, O_WRONLY | O_CREAT | (do_output_append? O_APPEND : O_TRUNC), 0666);
    if (fd < 0)
      perror("Cannot open output file");
    else
      redirect_stream(fd,  STDOUT_FILENO);
  }
}

/**
 * Replace fd_target with fd_source and close fd_source
 */
static void redirect_stream(int fd_source, int fd_target)
{
  if (fd_source == fd_target) /* no need to redirect */
    return;

  if (dup2(fd_source, fd_target)<0) /* replace target fd */
  {
    perror("redirect_stream:dup2");
    return;
  }
  if (close(fd_source)<0) /* close source fd */
  {
    perror("redirect_stream:close");
    return;
  }
}

/**
 * Wait for process to terminate, return desired return value
 */
static int check_wait(pid_t pid)
{
  int status;
  pid_t wait_result = waitpid(pid, &status, 0);
  if (wait_result == -1)
    perror("check_wait");
  else if (WIFEXITED(status))
  {
    trace("child %d exited with code %d", pid, WEXITSTATUS(status));
    return WEXITSTATUS(status);
  }
  else if (WIFSIGNALED(status))
  {
    trace("child %d killed by signal %d", pid, WTERMSIG(status));
  }
  else
    trace("child %d exited with unknown reason", pid);

  return 1;
}

/**
 * Builtin "cd" command
 */
static int builtin_cd(List argv)
{
  const char *path;
  size_t argc = list_size(argv);

  if (argc > 1)
  {
    fprintf(stderr, "cd: Only one argument expected\n");
    return 1;
  }
  else if (argc == 0)
  {
    path = getenv("HOME");
    if (path == NULL)
    {
      fprintf(stderr, "cd: HOME environment variable undefined\n");
      return 1;
    }
  }
  else
    path = list_head_str(argv);

  if (chdir(path) != 0)
  {
    perror("cd");
    return 1;
  }
  return 0;
}

