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


#ifndef CMDNODE_H
#define CMDNODE_H

#include "list.h"
#include "diagnostic.h"

/**
 * Command line program is represented by a binary tree.
 *
 * Each node is either an operator, or a command (leaf node).
 * A program is executed by recursing into the tree.
 */

/**
 * Node class
 * 
 * Classes form a bit mask to be able to specify sets of classes
 */
typedef enum
{
  CN_NULL       = 0x0,
  CN_COMMAND    = 0x1, 
  CN_CHAIN      = 0x2, 
  CN_AND        = 0x4,
  CN_OR         = 0x8,
  CN_PIPE       = 0x10,
  CN_SUBSHELL   = 0x20,
  CN_BACKGROUND = 0x40
} CommandNodeType;

/**
 * A node in internal program representation.
 */
typedef struct CommandNode 
{
  CommandNodeType type;

  char *command;

  List arguments;

  List input_files;
  List output_files;
  List output_append_files;

  List expression; 

  struct CommandNode *op1;
  struct CommandNode *op2;
} CommandNode;

/** 
 * CommandNode allocation frontends 
 */
CommandNode *cmdnode_command(char *command);
CommandNode *cmdnode_operator(char *operator);
CommandNode *cmdnode_subshell(List expression);

/**
 * Add stream redirection to a command node
 * type is one of ">", "<", ">>"
 */
void cmdnode_add_redirection(CommandNode *node, char *type, char *filename);

void cmdnode_free(CommandNode *node);
void cmdnode_free_recursive(CommandNode *root);

/** 
 * Transform a CommandNode tree so that for each subshell node
 * a list of its command is folded into a CommandNode tree.
 */
void cmdnode_unflatten(CommandNode *node, Diagnostic *diag);
#endif /* CMDNODE_H */

