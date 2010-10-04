#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "cmdline_parser.h"

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

typedef enum
{
  TT_NULL,
  TT_PIPE,
  TT_CHAIN,
  TT_BACKGROUND,
  TT_AND,
  TT_OR,
  TT_REDIRECT_INPUT,
  TT_REDIRECT_OUTPUT,
  TT_REDIRECT_OUTPUT_APPEND,
  TT_SUBSHELL_BEGIN,
  TT_SUBSHELL_END,
  TT_TEXT
} TokenType;

typedef struct 
{
  TokenType type;
  TokenClass class;
  char *str;
  ListNode *node;
} Token;

typedef struct
{
  List expr_stack;
  List current_expr;

  CommandNode *current_command;
  
  ListNode *args_begin;
  unsigned int args_n;
  TokenType redirect_type;
} ParserContext;


/**
 * FSM states
 */

typedef ParserState (*ParserStateHandler) (ParserContext *ctx, Token *token);

static ParserState state_command(ParserContext *ctx, Token *token);
static ParserState state_arguments(ParserContext *ctx, Token *token);
static ParserState state_redirections(ParserContext *ctx, Token *token);
static ParserState state_redirect_filename(ParserContext *ctx, Token *token);


/**
 * Token operations
 */

static TokenClass token_class(const char *token);
static TokenType token_type(const char *token);


/** 
 * FSM utility functions
 */

static void begin_command(ParserContext *ctx, char *command);
static void end_command(ParserContext *ctx);
static void add_argument(ParserContext *ctx, ListNode *arg);
static void add_redirection(ParserContext *ctx, char *filename);
static void add_operator(ParserContext *ctx, Token *token);
static void open_subshell(ParserContext *ctx);
static int close_subshell(ParserContext *ctx);

/**
 * Parser main loop
 */

static ParserState parser_process_token(ParserContext *ctx, ParserState state,
                                        char *token_str, ListNode *token_node);

CmdlineParserStatus parser_buildtree(CommandNode **root, ListNode *tokens)
{
  ParserContext ctx = 
  {
    .expr_stack = {NULL, NULL},
    .current_expr = {NULL, NULL},

    .current_command = NULL,

    .args_begin = NULL,
    .args_n = 0
  };

  ParserState state = ST_COMMAND;  

  /* Open top-level subshell */
  state = parser_process_token(&ctx, state, "(", NULL);

  while (tokens != NULL && state != ST_ERROR)
  {
    state = parser_process_token(&ctx, state, tokens->d.c_str, tokens);

    tokens = tokens->next;
  }

  /* Close top-level subshell */
  if (state != ST_ERROR)
    state = parser_process_token(&ctx, state, ")", NULL);

  if (state != ST_ERROR)
    return CMDLINE_OK;
  else
  {
    fprintf(stderr, "parser fail\n");
    return CMDLINE_PARSER_ERROR;
  }
}

static ParserState parser_process_token(ParserContext *ctx, ParserState state,
                                        char *token_str, ListNode *token_node)
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

  Token token = 
  {
    .node = token_node,
    .str = token_str,
    .type = token_type(token_str),
    .class = token_class(token_str)
  };

  /* debug trace */
  fprintf(stderr, "%d | <\"%s\" %d %d>\n", state, token.str, token.class, token.type);

  return state_handlers[state](ctx, &token);
}


/**
 * Parser states. Implementation.
 */

/* Expecting a command to start */
static ParserState state_command(ParserContext *ctx, Token *token)
{
  switch (token->class)
  {
    case TC_SUBSHELL_BEGIN:
      open_subshell(ctx);
      return ST_COMMAND;

    case TC_TEXT:
      begin_command(ctx, token->str);
      return ST_ARGUMENTS;

    default:
      return ST_ERROR;
  }
}

/* Just read a command, expecting args or command end */
static ParserState state_arguments(ParserContext *ctx, Token *token)
{
  switch (token->class)
  {
    /* is an argument */
    case TC_TEXT:
      add_argument(ctx, token->node);
      return ST_ARGUMENTS;

    /* otherwise, identical to ST_REDIRECTIONS */
    default:
      return state_redirections(ctx, token);
  }
}

static ParserState state_redirections(ParserContext *ctx, Token *token)
{
  switch (token->class)
  {
    case TC_REDIRECT:
      switch (token->type)
      {
        case TT_REDIRECT_INPUT:
        case TT_REDIRECT_OUTPUT:
        case TT_REDIRECT_OUTPUT_APPEND:
          fprintf(stderr, "expect redirection file\n");
          ctx->redirect_type = token->type;
          return ST_REDIRECT_FILENAME;

          /* impossible */
        default: 
          return ST_ERROR;
      }

    case TC_OPERATOR:
      end_command(ctx);
      add_operator(ctx, token);
      return ST_COMMAND;

    case TC_SUBSHELL_END:
      end_command(ctx);
      if (close_subshell(ctx))
        return ST_REDIRECTIONS;
      else
        return ST_ERROR;

    default:
      return ST_ERROR;
  }
}

/* Expecting a filename */
static ParserState state_redirect_filename(ParserContext *ctx, Token *token)
{
  switch (token->class)
  {
    case TC_TEXT:
      add_redirection(ctx, token->str);
      return ST_REDIRECTIONS;

    default:
      return ST_ERROR;
  }
}


/**
 * FSM utility functions. Implementation.
 */

static void begin_command(ParserContext *ctx, char *command)
{
  CommandNode *node = cmdnode_alloc(CN_COMMAND);
  node->command = command;

  ctx->args_begin = NULL;
  ctx->args_n = 0;

  list_append(&ctx->current_expr, list_node_alloc(node));
  ctx->current_command = node;

  fprintf(stderr, "begin command %s\n", command);
}

static void end_command(ParserContext *ctx)
{
  unsigned int argc;
  char **argv;
  ListNode *arg;
  unsigned int i;

  CommandNode *node = ctx->current_command;

  if (node->type != CN_COMMAND)
  {
    fprintf(stderr, "ending not-a-command %d\n", node->type);
    return;
  }

  argc = ctx->args_n + 1; /* + argv[0] */
  argv = (char **) malloc(sizeof(char *) * (argc + 1)); /* + argv[argc+1] == NULL */
  arg = ctx->args_begin;

  argv[0] = node->command;
  for(i=1; arg!=NULL; i++, arg=arg->next)
    argv[i] = arg->d.c_str;
  argv[argc] = NULL;

  ctx->current_command->argv = argv;

  ctx->current_command = NULL;
  ctx->args_begin = NULL;
  ctx->args_n = 0;
  
  fprintf(stderr, "end command %s\n", node->command);
}

static void add_argument(ParserContext *ctx, ListNode *arg)
{
  if (ctx->args_begin==NULL)
    ctx->args_begin = arg;
  ctx->args_n++;

  fprintf(stderr, "add argument %s\n", arg->d.c_str);
}

static void add_redirection(ParserContext *ctx, char *filename)
{
  List *list;
  CommandNode *node = ctx->current_command;

  switch (ctx->redirect_type)
  {
    case TT_REDIRECT_INPUT:
      list = &node->input_files;
      break;
    case TT_REDIRECT_OUTPUT:
      list = &node->output_files;
      break;
    case TT_REDIRECT_OUTPUT_APPEND:
      list = &node->output_append_files;
      break;

    default:
      /* this shouldn't happen */
      assert(0);
  }

  list_append(list, list_node_alloc(filename));

  fprintf(stderr, "redirect %d to %s\n", ctx->redirect_type, filename);
}

static void add_operator(ParserContext *ctx, Token *token)
{
  CommandNodeType cn_type;
  CommandNode *node;

  switch (token->type)
  {
    case TT_PIPE:
      cn_type = CN_PIPE;
      break;
    case TT_CHAIN:
      cn_type = CN_CHAIN;
      break;
    case TT_AND:
      cn_type = CN_AND;
      break;
    case TT_OR:
      cn_type = CN_OR;
      break;
    case TT_BACKGROUND:
      cn_type = CN_BACKGROUND;
      break;

    default:
      /* this shouldn't happen */
      assert(0);
  }

  node = cmdnode_alloc(cn_type);
  node->command = token->str;
  list_append(&ctx->current_expr, list_node_alloc(node));

  fprintf(stderr, "add operator %s\n", token->str);
}

static void open_subshell(ParserContext *ctx)
{
  list_push(&ctx->expr_stack, list_node_alloc(ctx->current_expr.root));
  list_init(&ctx->current_expr, NULL);

  fprintf(stderr, "open subshell\n");
}

static int close_subshell(ParserContext *ctx)
{
  CommandNode *node;
  if (ctx->expr_stack.root == NULL)
    return 0;

  node = cmdnode_alloc(CN_SUBSHELL);
  node->expression = ctx->current_expr.root;

  list_init(&ctx->current_expr, ctx->expr_stack.root->d.list);
  list_pop(&ctx->expr_stack);

  list_append(&ctx->current_expr, list_node_alloc(node));

  ctx->current_command = node;

  fprintf(stderr, "close subshell\n");

  return 1;
}


/**
 * Token operations. Implementation.
 */

static const struct token_definition
{
  const char *token;
  TokenClass class;
  TokenType type;
} token_definitions[] = 
{
  {"|",  TC_OPERATOR, TT_PIPE},
  {";",  TC_OPERATOR, TT_CHAIN},
  {"&",  TC_OPERATOR, TT_BACKGROUND},
  {"&&", TC_OPERATOR, TT_AND},
  {"||", TC_OPERATOR, TT_OR},
  {"<",  TC_REDIRECT, TT_REDIRECT_INPUT},
  {">",  TC_REDIRECT, TT_REDIRECT_OUTPUT},
  {">>", TC_REDIRECT, TT_REDIRECT_OUTPUT_APPEND},
  {"(",  TC_SUBSHELL_BEGIN, TT_SUBSHELL_BEGIN},
  {")",  TC_SUBSHELL_END, TT_SUBSHELL_END}
};
static const size_t n_token_definitions = 
  sizeof(token_definitions)/sizeof(struct token_definition);


static TokenClass token_class(const char *token)
{
  size_t i;

  if (token==NULL)
    return TC_NULL;
  
  for (i=0; i<n_token_definitions; i++)
    if (!strcmp(token_definitions[i].token, token))
      return token_definitions[i].class;
  return TC_TEXT;
}

static TokenType token_type(const char *token)
{
  size_t i;

  if (token==NULL)
    return TT_NULL;
  
  for (i=0; i<n_token_definitions; i++)
    if (!strcmp(token_definitions[i].token, token))
      return token_definitions[i].type;
  return TT_TEXT;
}


