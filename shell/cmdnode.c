#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cmdnode.h"
#include "debug.h"

static CommandNodeType operator_type(const char *operator);
static CommandNode *cmdnode_alloc(CommandNodeType type, char *command);
static int list_head2(List list, CommandNode **a, CommandNode **b);
static int list_head3(List list, CommandNode **a, CommandNode **b, CommandNode **c);
static List fold_list(List expression, unsigned int mask);
static int allow_incomplete(CommandNodeType type);


CommandNode *cmdnode_command(char *command)
{
  return cmdnode_alloc(CN_COMMAND, command);
}

CommandNode *cmdnode_operator(char *operator)
{
  return cmdnode_alloc(operator_type(operator), operator);
}

CommandNode *cmdnode_subshell(List expression)
{
  CommandNode *node = cmdnode_alloc(CN_SUBSHELL, NULL);
  node->expression = expression;

  return node;
}


void cmdnode_add_redirection(CommandNode *node, char *type, char *filename)
{
  if (!strcmp("<", type))
    node->input_files = list_push(node->input_files, filename);
  else if (!strcmp(">", type))
    node->output_files = list_push(node->output_files, filename);
  else if (!strcmp(">>", type))
    node->output_append_files = list_push(node->output_append_files, filename);
}

void cmdnode_free(CommandNode *node)
{
  if (node->expression != NULL)
  {
    List expr = node->expression;
    while (expr != NULL)
    {
      cmdnode_free_recursive(list_head_command(expr));
      expr = list_pop(expr);
    }
  }
  list_free(node->arguments);
  list_free(node->input_files);
  list_free(node->output_files);
  list_free(node->output_append_files);
  free(node);
}

void cmdnode_free_recursive(CommandNode *root)
{
  if (root->op1 != NULL)
    cmdnode_free_recursive(root->op1);

  if (root->op2 != NULL)
    cmdnode_free_recursive(root->op2);

  cmdnode_free(root);
}

/**
 * Command node tree unflattening
 * TODO: handle incorrect input
 */

void cmdnode_unflatten(CommandNode *node, CmdlineParserStatus *status)
{
  List expression;
  const unsigned int ops_pipe = CN_PIPE;
  const unsigned int ops_chain = CN_CHAIN | CN_BACKGROUND | CN_AND | CN_OR;

  *status = CMDLINE_OK;

  /* simple nodes don't need any assistance */
  if (node->type != CN_SUBSHELL || node->expression == NULL)
    return; 

  /* first, ensure all subexpressions are trees */
  expression = node->expression;
  while (expression != NULL)
  {
    cmdnode_unflatten(list_head_command(expression), status);
    /* stop on error */
    if (*status != CMDLINE_OK)
      return;
    expression = expression->next;
  }

  /* apply list folding */
  expression = node->expression;
  expression = fold_list(expression, ops_pipe);
  expression = fold_list(expression, ops_chain);
  
  /* list must have folded into a single item */
  if (expression->next == NULL) 
  {
    /* success */
    node->expression = NULL;
    node->op1 = list_head_command(expression);
    expression = list_pop(expression);
  }
  else
  {
    /* failure */
    *status = CMDLINE_EXPRESSION_ERROR;
    node->expression = expression;
  }
}

/** 
 * Utility functions. Implementation.
 */

/* get operator type by its string representation */
static CommandNodeType operator_type(const char *operator)
{
  static const struct operator_definition
  {
    const char *operator;
    CommandNodeType type;
  } operator_definitions[] = 
  {
    {"|",  CN_PIPE},
    {";",  CN_CHAIN},
    {"&",  CN_BACKGROUND},
    {"&&", CN_AND},
    {"||", CN_OR}
  };
  static const size_t n_operator_definitions = 
    sizeof(operator_definitions)/sizeof(struct operator_definition);

  size_t i;
  for (i=0; i<n_operator_definitions; i++)
    if (!strcmp(operator_definitions[i].operator, operator))
      return operator_definitions[i].type;

  return CN_NULL;
}

static CommandNode *cmdnode_alloc(CommandNodeType type, char *command)
{
  CommandNode *node = (CommandNode *) calloc(1, sizeof(CommandNode));
  node->type = type;
  node->command = command;

  return node; 
}

/* extract first 2 elements; return true if succeeded */
static int list_head2(List list, CommandNode **a, CommandNode **b)
{
  if (list==NULL || list->next==NULL)
    return 0; /* list too small */

  *a = list_head_command(list);
  *b = list_head_command(list->next);
  return 1;
}

/* extract first 3 elements; return true if succeeded */
static int list_head3(List list, CommandNode **a, CommandNode **b, CommandNode **c)
{
  if (list==NULL || list->next==NULL || list->next->next==NULL)
    return 0; /* list too small */

  *a = list_head_command(list);
  *b = list_head_command(list->next);
  *c = list_head_command(list->next->next);
  return 1;
}

/**
 * Transform expression, so that sequences like [l, OP, r] are folded into 
 * single subtree items (OP l, r). Operators are defined by mask, all operators
 * defined at the same time have same priority; left-associative.
 *
 * allow_incomplete(type) allows folding incomplete tail form: [l, OP] -> (OP l, null)
 */
static List fold_list(List expression, unsigned int mask)
{
  CommandNode *l, *op, *r;
  List result = NULL;

  while (list_head3(expression, &l, &op, &r))
  {
    /* 3-item form: l OP r */
    if (op->type & mask)
    {
      /* MATCH
       * expression: [L, Op, R | Tail] -> [Op | Tail]
       */
      op->op1 = l;
      op->op2 = r;
      expression = list_pop(list_pop(list_pop(expression)));
      expression = list_push(expression, op);
    }
    else
    {
      /* NO MATCH
       * expression: [L, Op, R | Tail] -> [R | Tail]
       * result:     Tail -> [Op, L | Tail]
       */
      expression = list_pop(list_pop(expression));
      result = list_push(list_push(result, l), op);
    }
  }

  if (list_head2(expression, &l, &op))
  {
    /* 2-item form: l OP */
    if (allow_incomplete(op->type) && (op->type & mask))
    {
      /* expression: [L, Op] -> []
       * result:     Tail -> [(Op, L) | Tail]
       */
      expression = list_pop(list_pop(expression));
      op->op1 = l;
      result = list_push(result, op);
    }
  }

  /* push expression tail into result list */
  while (expression != NULL)
  {
    result = list_push(result, list_head_command(expression));
    expression = list_pop(expression);
  }

  return list_reverse(result);
}
static int allow_incomplete(CommandNodeType type)
{
  if (type==CN_CHAIN || type==CN_BACKGROUND)
    return 1;
  else
    return 0;
}


