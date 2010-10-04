#ifndef SZLIST_H
#define SZLIST_H

/**
 * ListNode stores one node of a string list.
 *
 * ListNode is allocated and freed with list_node_alloc and list_node_free
 */
typedef struct ListNodeRef
{
  union ListNodeData {
    void *data;
    char *c_str;
    struct ListNodeRef *list;
    struct CommandNodeRef *command;
  } d; 
  struct ListNodeRef *next;
} ListNode;

/** 
 * List stores a reference to a list to ease appending elements to it.
 *
 * Memory is allocated by caller (possibly static)
 */
typedef struct ListRef
{
  ListNode *root;
  ListNode *tail;
} List;

/**
 * Initialize List with existing list data (possibly NULL). 
 */
void list_init(List *list, ListNode *list_data);
/**
 * Free all nodes in List. Strings are not freed, only discarded.
 */
void list_destroy(List *list);
/**
 * Append a list (or a single node) to List.
 */
void list_append(List *list, ListNode *node);
void list_push(List *list, ListNode *node);
void list_pop(List *list);


/**
 * Allocate a new list node.
 *
 * c_str is initialized with default_str
 * next is initialized with NULL
 */
ListNode *list_node_alloc(void *default_data);
/**
 * Free memory used by a list node.
 * String data is not freed, only discarded.
 */
void list_node_free(ListNode *node);

#endif /* SZLIST_H */

