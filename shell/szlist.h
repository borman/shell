#ifndef SZLIST_H
#define SZLIST_H

/**
 * SzListNode stores one node of a string list.
 *
 * SzListNode is allocated and freed with szlist_node_alloc and szlist_node_free
 */
typedef struct SzListNodeRef
{
  char *c_str;
  struct SzListNodeRef *next;
} SzListNode;

/** 
 * SzList stores a reference to a list to ease appending elements to it.
 *
 * Memory is allocated by caller (possibly static)
 */
typedef struct 
{
  SzListNode *root;
  SzListNode *tail;
} SzList;

/**
 * Initialize SzList with existing list data (possibly NULL). 
 */
void szlist_init(SzList *list, SzListNode *list_data);
/**
 * Free all nodes in SzList. Strings are not freed, only discarded.
 */
void szlist_destroy(SzList *list);
/**
 * Append a list (or a single node) to SzList.
 */
void szlist_append(SzList *list, SzListNode *node);


/**
 * Allocate a new list node.
 *
 * c_str is initialized with default_str
 * next is initialized with NULL
 */
SzListNode *szlist_node_alloc(char *default_str);
/**
 * Free memory used by a list node.
 * String data is not freed, only discarded.
 */
void szlist_node_free(SzListNode *node);

#endif /* SZLIST_H */

