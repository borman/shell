#include <stdlib.h>

#include "list.h"

void list_init(List *list, ListNode *list_data)
{
  list->root = list_data;
  list->tail = list_data;

  /* traverse list to its end */
  if (list->tail != NULL)
    while (list->tail->next!=NULL)
      list->tail = list->tail->next;
}

void list_destroy(List *list)
{
  ListNode *node = list->root;
  while (node != NULL)
  {
    ListNode *t = node;
    node = node->next;
    list_node_free(t);
  }
}

void list_append(List *list, ListNode *node)
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

void list_push(List *list, ListNode *node)
{
  node->next = list->root;
  list->root = node;
  if (list->tail==NULL)
    list->tail = node;
}

void list_pop(List *list)
{
  ListNode *tmp;

  if (list->tail == list->root)
    list->tail = NULL;

  tmp = list->root;
  list->root = list->root->next;
  list_node_free(tmp);
}

ListNode *list_node_alloc(void *default_data)
{
  ListNode *node = (ListNode *)malloc(sizeof(ListNode));

  node->d.data = default_data;
  node->next = NULL;

  return node;
}

void list_node_free(ListNode *node)
{
  free(node);
}


