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
  ST_OPERATOR,
  ST_ERROR
} LexerState;

/* Lexer FSM main loop */
Buffer *lexer_split(const char *src, Diagnostic *diag)
{
  LexerState state = ST_WHITESPACE;
  LexerState prev_state;
  const size_t length = strlen(src);
  Buffer *tokens = buffer_alloc();
  size_t i;
  char op = 0;

  /* terminating zero is handled like a normal character */
  for (i=0; state!=ST_ERROR && i<length+1; i++)
  {
    const char c = src[i];
    prev_state = state;

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
          state = ST_QUOTE;
        else if (c=='\\') /* start escape-sequence, new token */
          state = ST_ESCAPE;
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
        /* else: whitespace -> ignore */
        break;

      case ST_QUOTE:
        if (c=='"') /* end a quoted string */
          state = ST_WORD;
        else if (c=='\\') /* start escape-sequence */
          state = ST_QUOTE_ESCAPE;
        else if (c!='\0') /* append a char from inside quotes to token */
          buffer_putchar(tokens, c);
        else /* '\0' -> set error flag */
          state = ST_ERROR;
        break;

      case ST_WORD:
        if (c=='\0' || isspace(c)) /* end a token */
        {
          state = ST_WHITESPACE;
          buffer_putchar(tokens, '\0');
        }
        else if (c=='"') /* start a quoted string */
          state = ST_QUOTE;
        else if (c=='\\') /* start escape-sequence */
          state = ST_ESCAPE; 
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
          buffer_putchar(tokens, c);
        break;

      case ST_ESCAPE:
        if (c!='\0')
        {
          state = ST_WORD;
          buffer_putchar(tokens, c);
        }
        else /* '\0' -> set error flag */
        {
          state = ST_ERROR;
        }
        break;

      case ST_QUOTE_ESCAPE:
        if (c!='\0')
        {
          state = ST_QUOTE;
          buffer_putchar(tokens, c);
        }
        else /* '\0' -> set error flag */
          state = ST_ERROR;
        break;

      default:
        break;
    }    
  }

  /* Detect lexing errors */
  if (state == ST_ERROR)
  {
    buffer_free(tokens);

    diag->error = 1;
    switch (prev_state)
    {
      case ST_QUOTE:
        diag->error_message = "Unfinished quote string";
        break;

      case ST_ESCAPE:
      case ST_QUOTE_ESCAPE:
        diag->error_message = "Unfinished escape sequence";
        break;

      default:
        break;
    }
    return NULL;
  }
  else 
  {
    diag->error = 0;
    return tokens;
  }
}


