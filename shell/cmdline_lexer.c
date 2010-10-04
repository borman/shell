#include <strings.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "cmdline_lexer.h"

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
 *
 * Implemented grammar features:
 * - enclosing verbatim strings in double quotes
 * - escaping symbols in quoted string and plain text ('\' + symbol) -> symbol
 * - separating single-char operators in their own tokens
 *
 * FIXME: need to save token origin, e.g. to consider ">" a regular string while
 *   > is a redirection operator
 */ 

/* character class tests */
#define is_simple_operator(c) ((c)==';' || (c)=='<' || (c)=='(' || (c)==')')
#define is_double_operator(c) ((c)=='>' || (c)=='|' || (c)=='&')

typedef enum
{
  ST_WHITESPACE,
  ST_WORD,
  ST_QUOTE,
  ST_ESCAPE,
  ST_QUOTE_ESCAPE,
  ST_OPERATOR
} LexerState;

/* Lexer FSM main loop */
Buffer *lexer_split(const char *src, CmdlineParserStatus *result)
{
  LexerState state = ST_WHITESPACE;
  const size_t length = strlen(src);
  Buffer *tokens = buffer_alloc();
  size_t i;
  char op = 0;

  /* terminating zero is handled like a normal character */
  for (i=0; i<length+1; i++)
  {
    const char c = src[i];

    switch (state)
    {
      case ST_OPERATOR:
        state = ST_WHITESPACE;
        if (c==op)
        {
          /* double operator */
          buffer_putchar(tokens, op);
          buffer_putchar(tokens, op);
          buffer_putchar(tokens, '\0');
          break;
        }
        else
        {
          /* single operator */
          buffer_putchar(tokens, op);
          buffer_putchar(tokens, '\0');
        }
        /* no break (intended) -- fall back to whitespace case */

      case ST_WHITESPACE:
        if (c=='"') /* start a quoted string, new token */
        {
          state = ST_QUOTE;
        }
        else if (c=='\\') /* start escape-sequence, new token */
        {
          state = ST_ESCAPE;
        }
        else if (is_simple_operator(c))
        {
          /* a new token for operator */
          buffer_putchar(tokens, c);
          buffer_putchar(tokens, '\0');
        }
        else if (is_double_operator(c))
        {
          state = ST_OPERATOR;

          op = c;
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
        else if (is_simple_operator(c))
        {
          state = ST_WHITESPACE;

          /* a new token for operator */
          buffer_putchar(tokens, '\0');
          buffer_putchar(tokens, c);
          buffer_putchar(tokens, '\0');
        }
        else if (is_double_operator(c))
        {
          state = ST_OPERATOR;

          buffer_putchar(tokens, '\0');
          op = c;
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
      *result = CMDLINE_LEX_UNBALANCED_QUOTE;
      goto L_ERROR;

    case ST_ESCAPE:
    case ST_QUOTE_ESCAPE:
      *result = CMDLINE_LEX_UNFINISHED_ESCAPE;
      goto L_ERROR;

    default:
      *result = CMDLINE_OK;
      return tokens;
  }

L_ERROR:
  buffer_free(tokens);
  return NULL;
}


