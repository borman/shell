#include "cmdline.h"
#include <strings.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/**
 * Command line lexer
 * Splits command line into a list of words
 * Implemented as a finite state machine
 *
 * Tokens are stored internally as a sequence of 0-terminated strings in a 
 * single buffer.
 */

typedef enum
{
  ST_WHITESPACE,
  ST_WORD,
  ST_QUOTE
} LEXER_STATE;

typedef struct 
{
  Buffer *strings;
  SzList *tokens;
} LexerContext;

/* Lexer operations to clean up FSM inner code */

static void lexer_begin_word(LexerContext *context)
{
  SzListNode *node = szlist_node_alloc(buffer_end(context->strings));
  szlist_append(context->tokens, node);
}

static void lexer_end_word(LexerContext *context)
{
  buffer_putchar(context->strings, '\0'); /* separate tokens */
}

static void lexer_append_char(LexerContext *context, char c)
{
  buffer_putchar(context->strings, c);
}

/* Lexer FSM main loop */
static CMDLINE_PARSER_STATUS lexer_split(LexerContext *context, const char *src)
{
  LEXER_STATE state = ST_WHITESPACE;
  const size_t length = strlen(src);
  size_t i;

  /* terminating zero is handled like a normal character */
  for (i=0; i<length+1; i++)
  {
    const char c = src[i];
    switch (state)
    {
      case ST_WHITESPACE:
        if (c=='"') /* start a quoted string, new token */
        {
          state = ST_QUOTE;

          lexer_begin_word(context);
        }
        else if (c!='\0' && !isspace(c)) /* start a new token */
        {
          state = ST_WORD;

          lexer_begin_word(context);
          lexer_append_char(context, c);
        }
        /* whitespace -> ignore */
        break;

      case ST_QUOTE:
        if (c=='"') /* end a quoted string */
        {
          state = ST_WORD;
        } 
        else if (c!='\0') /* append a char from inside quotes to token */
        {
          lexer_append_char(context, c);
        }
        /* '\0' -> ignore (will set error flag later) */
        break;

      case ST_WORD:
        if (c=='\0' || isspace(c)) /* end a token */
        {
          state = ST_WHITESPACE;

          lexer_end_word(context);
        }
        else if (c=='"') /* start a quoted string */
        {
          state = ST_QUOTE;
        }
        else /* append a char to token */
        {
          lexer_append_char(context, c);
        }
    }    
  }

  /* Detect lexing errors */
  if (state == ST_QUOTE)
    return CMDLINE_LEX_UNBALANCED_QUOTE;

  return CMDLINE_OK;
}

CommandTree *cmdline_lex(const char *cmdline)
{
  CommandTree *tree = (CommandTree *)malloc(sizeof(CommandTree));
  LexerContext context; 

  tree->strings = buffer_alloc();
  szlist_init(&tree->tokens, NULL);

  context.strings = tree->strings;
  context.tokens = &tree->tokens;

  tree->status = lexer_split(&context, cmdline);

  return tree;
}

void cmdline_free(CommandTree *tree)
{
  buffer_free(tree->strings);
  szlist_destroy(&tree->tokens);

  free(tree);
}

