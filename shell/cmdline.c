#include "cmdline.h"
#include <strings.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

CommandTree *cmdline_lex(const char *cmdline)
{
  enum
  {
    ST_WHITESPACE,
    ST_WORD,
    ST_QUOTE
  } state = ST_WHITESPACE;
  const size_t length = strlen(cmdline);
  size_t i;
  SzListNode *node = NULL;

  CommandTree *tree = (CommandTree *)malloc(sizeof(CommandTree));

  szlist_init(&tree->tokens, NULL);
  tree->status = CMDLINE_OK;
  tree->stringdata = buffer_alloc();

  /* terminating zero is handled like a normal character */
  for (i=0; i<length+1; i++)
  {
    const char c = cmdline[i];
    switch (state)
    {
      case ST_WHITESPACE:
        if (c=='"')
        {
          state = ST_QUOTE;
          /* start word */
          node = szlist_node_alloc(buffer_end(tree->stringdata));
        }
        else if (c!='\0' && !isspace(c))
        {
          state = ST_WORD;
          /* start word */
          node = szlist_node_alloc(buffer_end(tree->stringdata));
          /* append char */
          buffer_putchar(tree->stringdata, c);
        }
        break;

      case ST_QUOTE:
        if (c=='"')
        {
          state = ST_WORD;
        } 
        else if (c!='\0')
        {
          /* append char */
          buffer_putchar(tree->stringdata, c);
        }
        break;

      case ST_WORD:
        if (c=='\0' || isspace(c))
        {
          state = ST_WHITESPACE;
          /* finish word */
          buffer_putchar(tree->stringdata, '\0');
          szlist_append(&tree->tokens, node);
          node = NULL;
        }
        else if (c=='"')
        {
          state = ST_QUOTE;
        }
        else 
        {
          /* append char */
          buffer_putchar(tree->stringdata, c);
        }
    }    
  }

  /* Detect parsing errors */
  if (state == ST_QUOTE)
  {
    tree->status = CMDLINE_LEX_UNBALANCED_QUOTE;
    if (node != NULL)
    {
      free(node);
      node = NULL;
    }
  }

  return tree;
}

void cmdline_parse(CommandTree *tree)
{
}

void cmdline_free(CommandTree *tree)
{
  buffer_free(tree->stringdata);
  szlist_destroy(&tree->tokens);
  free(tree);
}

