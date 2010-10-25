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


#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

/**
 * Buffer: a dynamic-allocated resizable string.
 *
 * Operations supported:
 * - erasing all contents
 * - appending a char to the end
 * - assigning a C string
 * - retrieving a pointer to buffer contents
 * - retrieving a pointer to buffer contents' terminating zero
 *
 * It is guaranteed that there is a terminating zero after buffer contents
 * (i.e. its contents are a valid C string).
 * Is is assumed that buffer contents are not changed outside the buffer.
 */
typedef struct
{
  /* public */
  char *c_str;
  
  /* private */
  size_t memsize;
  size_t length;
} Buffer;

/* Allocate a new buffer instance */
Buffer *buffer_alloc();
/* Free resourced used by buffer */
void buffer_free(Buffer *buffer);

/* Erase all buffer contents (set them to an empty string */
void buffer_clear(Buffer *buffer);
/* Append a char to the end of the buffer */
void buffer_putchar(Buffer *buffer, char c);
/* Assign buffer contents to a C string */
void buffer_copy(Buffer *dest, const char *src);

/* Retrieve a pointer to buffer's terminating zero.
 *
 * This pointer can be used as a place where the next char appended is
 * going to appear.
 */
char *buffer_end(Buffer *buf);

#endif


