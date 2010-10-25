/*
 * Copyright 2010, Mikhail "borman" Borisov <borisov.mikhail@gmail.com>
 *
 * This file is part of borman's model shell.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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

