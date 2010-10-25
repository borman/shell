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


#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#include "list.h"
#include "cmdnode.h"
#include "colors.h"

/**
 * Print contents of a string list as a column 
 */
void debug_dump_szlist(FILE *file, List list);
void debug_dump_cmdnode(FILE *file, CommandNode *node);
void debug_dump_expression_graph(CommandNode *expr);

void trace(const char *format, ...);

#endif /* DEBUG_H */
