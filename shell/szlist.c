#include "szlist.h"
#include <stdlib.h>

void szlist_init(SzList *list, SzListNode *list_data)
{
  list->root = list_data;
  list->tail = list_data;

  /* traverse list to its end */
  if (list->tail != NULL)
    while (list->tail->next!=NULL)
      list->tail = list->tail->next;
}

void szlist_destroy(SzList *list)
{
  SzListNode *node = list->root;
  while (node != NULL)
  {
    SzListNode *t = node;
    node = node->next;
    szlist_node_free(t);
  }
}

void szlist_append(SzList *list, SzListNode *node)
{
  if (list->root == NULL)
  {
    list->root = node;
    list->tail = node;
  }
  else
  {
    list->tail->next = node;
    list->tail = node;
  }

  /* traverse list to its end */
  if (list->tail != NULL)
    while (list->tail->next!=NULL)
      list->tail = list->tail->next;
}

SzListNode *szlist_node_alloc(char *default_str)
{
  SzListNode *node = (SzListNode *)malloc(sizeof(SzListNode));

  node->c_str = default_str;
  node->next = NULL;

  return node;
}

void szlist_node_free(SzListNode *node)
{
  free(node);
}


