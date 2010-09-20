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

  CommandNode *node = NULL;
  CommandNode *tail = NULL;

  CommandTree *tree = (CommandTree *)malloc(sizeof(CommandTree));
  tree->root = NULL;
  tree->errno = 0;
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
          node = cmdnode_alloc(COMMANDNODE_WORD);
          node->str = tree->stringdata->c_str + tree->stringdata->length;
        }
        else if (c!='\0' && !isspace(c))
        {
          state = ST_WORD;
          /* start word */
          node = cmdnode_alloc(COMMANDNODE_WORD);
          node->str = tree->stringdata->c_str + tree->stringdata->length;
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
          if (tree->root == NULL)
            tree->root = node;
          if (tail != NULL)
            tail->next = node;
          tail = node;
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
  if (state != ST_WHITESPACE)
  {
    tree->errno = 1;
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
  free(tree);
}

int cmdline_exec(CommandNode *node)
{
  return -1;
}

