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


#ifndef CMDLINE_H
#define CMDLINE_H

#include "list.h"
#include "buffer.h"
#include "cmdnode.h"
#include "diagnostic.h"

typedef struct 
{
  Diagnostic diag;   /* Error flag and description */

  List tokens;       /* Lexer output */
  CommandNode *tree; /* Execution tree */

  Buffer *strings;   /* Token storage */
} Program;

/** 
 * Perform lexical parsing of cmdline.
 * 
 * Returns a non-NULL pointer in any case.
 */
Program *cmdline_parse(const char *cmdline);

/**
 * Free resources allocated for parsed command line string.
 */
void cmdline_free(Program *tree);

#endif /* CMDLINE_H */

