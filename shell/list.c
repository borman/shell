#include <stdlib.h>
#include <assert.h>

#include "list.h"

static List list_node_alloc(void *default_data);
static void list_node_free(List node);

List list_push(List list, void *data)
{
  List new_head = list_node_alloc(data);
  new_head->next = list;
  return new_head;
}

List list_pop(List list)
{
  List head;

  assert(list != NULL);

  head = list;
  list = list->next;
  list_node_free(head);
  return list;
}

List list_reverse(List list)
{
  List prev = NULL;
  while (list != NULL)
  {
    List next = list->next;
    list->next = prev;
    prev = list;
    list = next;
  }
  return prev;
}


void *list_head(List list)
{
  assert(list != NULL);

  return list->data;
}


void list_free(List list)
{
  while (list != NULL)
  {
    List head = list;
    list = list->next;
    list_node_free(head);
  }
}

size_t list_size(List list)
{
  size_t size = 0;
  while (list != EmptyList)
  {
    size++;
    list = list->next;
  }
  return size;
}


/**
 * Node allocation
 */

static List list_node_alloc(void *default_data)
{
  List node = (List)malloc(sizeof(List));

  node->data = default_data;
  node->next = NULL;

  return node;
}

static void list_node_free(List node)
{
  free(node);
}


