#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "cmdline_parser.h"
#include "colors.h"
#include "debug.h"

/**
 * Command line parser
 */

typedef enum
{
  ST_COMMAND = 0,
  ST_ARGUMENTS,
  ST_REDIRECTIONS,
  ST_REDIRECT_FILENAME,
  ST_ERROR
} ParserState;

typedef enum
{ 
  TC_NULL,
  TC_OPERATOR,
  TC_REDIRECT,
  TC_SUBSHELL_BEGIN,
  TC_SUBSHELL_END,
  TC_TEXT
} TokenClass;

typedef struct
{
  List expr_stack;
  List current_expr;

  CommandNode *current_command;
  
  char *redirect_type;
} ParserContext;


/**
 * FSM states
 */

typedef ParserState (*ParserStateHandler) (ParserContext *ctx, char *token);

static ParserState state_command(ParserContext *ctx, char *token);
static ParserState state_arguments(ParserContext *ctx, char *token);
static ParserState state_redirections(ParserContext *ctx, char *token);
static ParserState state_redirect_filename(ParserContext *ctx, char *token);


/**
 * General utility functions
 */

static TokenClass token_class(const char *token);
static void fix_arguments_order(CommandNode *node);


/** 
 * FSM utility functions
 */

static void begin_command(ParserContext *ctx, char *command);
static void add_argument(ParserContext *ctx, char *arg);
static void add_redirection(ParserContext *ctx, char *filename);
static void add_operator(ParserContext *ctx, char *token);
static void open_subshell(ParserContext *ctx);
static int close_subshell(ParserContext *ctx);

/**
 * Parser main loop
 */

static ParserState parser_process_token(ParserContext *ctx, 
                                        ParserState state, char *token_str);

CommandNode *parser_buildtree(List tokens, CmdlineParserStatus *status)
{
  ParserContext ctx = {EmptyList, EmptyList, NULL, NULL};

  ParserState state = ST_COMMAND;  

  /* Open top-level subshell */
  state = parser_process_token(&ctx, state, "(");

  while (tokens != NULL && state != ST_ERROR)
  {
    state = parser_process_token(&ctx, state, list_head_str(tokens));

    tokens = tokens->next;
  }

  /* Close top-level subshell */
  if (state != ST_ERROR)
    state = parser_process_token(&ctx, state, ")");

  if (state != ST_ERROR && state != ST_REDIRECT_FILENAME)
  {
    *status = CMDLINE_OK;
    fix_arguments_order(ctx.current_command);
    return ctx.current_command;
  }
  else
  {
    fprintf(stderr, "parser fail\n");
    *status = CMDLINE_PARSER_ERROR;
    return NULL;
    /* FIXME: memory leak */
  }
}

static ParserState parser_process_token(ParserContext *ctx, 
                                        ParserState state, char *token)
{
  /* Link states to their handlers */
  static const ParserStateHandler state_handlers[] = {
    state_command,
    state_arguments,
    state_redirections,
    state_redirect_filename
      /* ST_ERROR 
       * is a terminal state, thus it has no handler
       */
  };

  /* debug trace */
  fprintf(stderr, TERM_FG_CYAN TERM_BOLD "%d | <\"%s\" %d>\n" TERM_NORMAL,
          state, token, token_class(token));

  return state_handlers[state](ctx, token);
}


/**
 * Parser states. Implementation.
 */

/* Expecting a command to start */
static ParserState state_command(ParserContext *ctx, char *token)
{
  switch (token_class(token))
  {
    case TC_SUBSHELL_BEGIN:
      open_subshell(ctx);
      return ST_COMMAND;

    case TC_SUBSHELL_END:
      if (close_subshell(ctx))
        return ST_REDIRECTIONS;
      else
        return ST_ERROR;

    case TC_TEXT:
      begin_command(ctx, token);
      return ST_ARGUMENTS;

    default:
      return ST_ERROR;
  }
}

/* Just read a command, expecting args or command end */
static ParserState state_arguments(ParserContext *ctx, char *token)
{
  if (token_class(token) == TC_TEXT)
  {
    /* is an argument */
    add_argument(ctx, token);
    return ST_ARGUMENTS;
  }

  /* otherwise, identical to ST_REDIRECTIONS */
  return state_redirections(ctx, token);
}

static ParserState state_redirections(ParserContext *ctx, char *token)
{
  switch (token_class(token))
  {
    case TC_REDIRECT:
      ctx->redirect_type = token;
      return ST_REDIRECT_FILENAME;

    case TC_OPERATOR:
      add_operator(ctx, token);
      return ST_COMMAND;

    case TC_SUBSHELL_END:
      if (close_subshell(ctx))
        return ST_REDIRECTIONS;
      else
        return ST_ERROR;

    default:
      return ST_ERROR;
  }
}

/* Expecting a filename */
static ParserState state_redirect_filename(ParserContext *ctx, char *token)
{
  switch (token_class(token))
  {
    case TC_TEXT:
      add_redirection(ctx, token);
      return ST_REDIRECTIONS;

    default:
      return ST_ERROR;
  }
}


/**
 * FSM utility functions. Implementation.
 */

static void begin_command(ParserContext *ctx, char *command_str)
{
  CommandNode *command = cmdnode_command(command_str);

  ctx->current_expr = list_push(ctx->current_expr, command);
  ctx->current_command = command;

  fprintf(stderr, "begin command %s\n", command_str);
}

static void add_argument(ParserContext *ctx, char *arg)
{
  ctx->current_command->arguments = 
    list_push(ctx->current_command->arguments, arg);

  fprintf(stderr, "add argument %s\n", arg);
}

static void add_redirection(ParserContext *ctx, char *filename)
{
  cmdnode_add_redirection(ctx->current_command, ctx->redirect_type, filename);

  fprintf(stderr, "redirect %s to %s\n", ctx->redirect_type, filename);
}

static void add_operator(ParserContext *ctx, char *operator)
{
  ctx->current_expr = list_push(ctx->current_expr, 
      cmdnode_operator(operator));

  fprintf(stderr, "add operator %s\n", operator);
}

static void open_subshell(ParserContext *ctx)
{
  ctx->expr_stack = list_push(ctx->expr_stack, ctx->current_expr);
  ctx->current_expr = EmptyList;

  fprintf(stderr, "open subshell\n");
}

static int close_subshell(ParserContext *ctx)
{
  CommandNode *subshell;

  if (ctx->expr_stack == EmptyList)
    return 0;

  subshell = cmdnode_subshell(list_reverse(ctx->current_expr));
  fprintf(stderr, "subshell node:\n");
  debug_dump_cmdnode(stderr, subshell);
  fprintf(stderr, "\n");

  ctx->current_expr = list_push(list_head_list(ctx->expr_stack), subshell);
  ctx->expr_stack = list_pop(ctx->expr_stack);

  ctx->current_command = subshell;

  fprintf(stderr, "close subshell\n");

  return 1;
}


/**
 * Token operations. Implementation.
 */

static const struct token_class_definition
{
  TokenClass class;
  const char *tokens[6];
} token_class_definitions[] = 
{
  {TC_OPERATOR,       {"|", ";", "&", "&&", "||", NULL}},
  {TC_REDIRECT,       {"<", ">", ">>", NULL}},
  {TC_SUBSHELL_BEGIN, {"(", NULL}},
  {TC_SUBSHELL_END,   {")", NULL}}
};
static const size_t n_token_class_definitions = 
  sizeof(token_class_definitions)/sizeof(struct token_class_definition);
  
static int str_in_set(const char *str, const char * const *set)
{
  size_t i;
  for (i=0; set[i]!=NULL; i++)
    if (!strcmp(str, set[i]))
      return 1;
  return 0;
}

static TokenClass token_class(const char *token)
{
  size_t i;
  
  for (i=0; i<n_token_class_definitions; i++)
    if (str_in_set(token, token_class_definitions[i].tokens))
      return token_class_definitions[i].class;

  return TC_TEXT;
}


static void fix_arguments_order(CommandNode *node)
{
  List subnodes;

  if (node == NULL)
    return;

  switch (node->type)
  {
    case CN_COMMAND:
      node->arguments = list_reverse(node->arguments);
      break;

    case CN_SUBSHELL:
      subnodes = node->expression;
      while (subnodes != EmptyList)
      {
        fix_arguments_order(list_head_command(subnodes));
        subnodes = subnodes->next;
      }
      break;

    default:
      break;
  }
}

