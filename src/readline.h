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


#ifndef READLINE_H
#define READLINE_H

#include "buffer.h"

/* Prompt result */
typedef enum
{
  READLINE_NEWLINE,
  READLINE_EOF
} READLINE_RESULT;

/**
 * Print a prompt and read input up to end of line or
 * end of file.
 *
 * Result is stored in buffer.
 * Buffer MUST be pre-allocated. Buffer contents will be erased.
 *
 * If prompt is NULL, it is not printed.
 */
READLINE_RESULT readline_run(const char *prompt, Buffer *buf);

#endif /* READLINE_H */

