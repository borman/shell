#ifndef BUFFER_H
#define BUFFER_H

#include <sys/types.h>

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


