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
 *
 * Lexing is performed in 2 passes:
 * 1) make a chain of 0-separated strings
 * 2) make a list of strings
 */

typedef enum
{
  ST_WHITESPACE,
  ST_WORD,
  ST_QUOTE,
  ST_ESCAPE,
  ST_QUOTE_ESCAPE
} LEXER_STATE;

/* Lexer FSM main loop */
static CMDLINE_PARSER_STATUS lexer_split(Buffer *tokens, const char *src)
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
        }
        else if (c=='\\') /* start escape-sequence, new token */
        {
          state = ST_ESCAPE;
        }
        else if (c!='\0' && !isspace(c)) /* start a new token */
        {
          state = ST_WORD;

          buffer_putchar(tokens, c);
        }
        /* whitespace -> ignore */
        break;

      case ST_QUOTE:
        if (c=='"') /* end a quoted string */
        {
          state = ST_WORD;
        } 
        else if (c=='\\') /* start escape-sequence */
        {
          state = ST_QUOTE_ESCAPE;
        }
        else if (c!='\0') /* append a char from inside quotes to token */
        {
          buffer_putchar(tokens, c);
        }
        /* '\0' -> ignore (will set error flag later) */
        break;

      case ST_WORD:
        if (c=='\0' || isspace(c)) /* end a token */
        {
          state = ST_WHITESPACE;

          buffer_putchar(tokens, '\0');
        }
        else if (c=='"') /* start a quoted string */
        {
          state = ST_QUOTE;
        }
        else if (c=='\\') /* start escape-sequence */
        {
          state = ST_ESCAPE; 
        }
        else /* append a char to token */
        {
          buffer_putchar(tokens, c);
        }
        break;

      case ST_ESCAPE:
        if (c!='\0')
        {
          state = ST_WORD;

          buffer_putchar(tokens, c);
        }
        /* '\0' -> ignore (will set error flag later) */
        break;

      case ST_QUOTE_ESCAPE:
        if (c!='\0')
        {
          state = ST_QUOTE;

          buffer_putchar(tokens, c);
        }
        /* '\0' -> ignore (will set error flag later) */
        break;
    }    
  }

  /* Detect lexing errors */
  switch (state)
  {
    case ST_QUOTE:
      return CMDLINE_LEX_UNBALANCED_QUOTE;

    case ST_ESCAPE:
    case ST_QUOTE_ESCAPE:
      return CMDLINE_LEX_UNFINISHED_ESCAPE;

    default:
      return CMDLINE_OK;
  }
}

/* Make a list out of a chain of strings.
 * *list is considered uninitialized */
static void lexer_make_list(SzList *list, const Buffer *strings)
{
  size_t pos = 0;

  szlist_init(list, NULL);
  while (pos<strings->length)
  {
    size_t delta = strlen(strings->c_str+pos);

    szlist_append(list, szlist_node_alloc(strings->c_str+pos));
    pos += delta+1;
  }
}

/* ==== */

CommandTree *cmdline_parse(const char *cmdline)
{
  CommandTree *tree = (CommandTree *)malloc(sizeof(CommandTree));

  tree->strings = buffer_alloc();

  tree->status = lexer_split(tree->strings, cmdline);
  lexer_make_list(&tree->tokens, tree->strings);

  return tree;
}

void cmdline_free(CommandTree *tree)
{
  buffer_free(tree->strings);
  szlist_destroy(&tree->tokens);

  free(tree);
}

