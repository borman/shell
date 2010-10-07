#ifndef LIST_H
#define LIST_H

#define EmptyList (NULL)

typedef struct ListNodeRef
{
  void *data;
  struct ListNodeRef *next;
} ListNode;
typedef ListNode *List;

List list_push(List list, void *data);
List list_pop(List list);
List list_reverse(List list);

void *list_head(List list);

void list_free(List list);

size_t list_size(List list);

#define list_head_str(list) ((char *)list_head(list))
#define list_head_list(list) ((List)list_head(list))
#define list_head_command(list) ((struct CommandNodeRef *)list_head(list))

#endif /* LIST_H */

