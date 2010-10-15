#ifndef LIST_H
#define LIST_H

/**
 * Lists module.
 *
 * Main entity is List type. 
 * No memory management is exposed.
 * All subroutines work listwise.
 *
 * User is not expected to modify list nodes' contents in any way
 * except by using this module's API.
 */

typedef struct ListNode
{
  void *data;
  struct ListNode *next;
} ListNode;
typedef ListNode *List;

/**
 * Insert a new element before list head.
 * Returns a new list.
 */
List list_push(List list, void *data);
/**
 * Remove list head.
 * Returns a new list.
 */
List list_pop(List list);
/**
 * Reverse list.
 * Returns a new pointer to this list, old pointer becomes invalid.
 */
List list_reverse(List list);

/** 
 * Extract list's head.
 */
void *list_head(List list);

/** 
 * Free memory used by this list.
 */
void list_free(List list);

/**
 * Calculate list size
 */
size_t list_size(List list);

/**
 * Typed head extraction macros
 */
#define list_head_str(list) ((char *)list_head(list))
#define list_head_list(list) ((List)list_head(list))
#define list_head_command(list) ((struct CommandNode *)list_head(list))

#endif /* LIST_H */

