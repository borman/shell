#include "buffer.h"
#include <stdlib.h>
#include <string.h>

/* Size of the buffer, allocated at its creation */
const size_t BUF_ALLOC_BASE = 16;

/* Which size should we grow to? */
static size_t size_next(size_t current)
{
  return current*2;
}

/* Which size should we grow to to make it greater or equal than required? */
static size_t size_enough(size_t current, size_t required)
{
  while (current<required)
    current = size_next(current);
  return current;
}

/* Ensure that buffer has enough memory allocated to store a tring of required 
 * length (not counting terminating zero */
static void buffer_ensure_size(Buffer *buf, size_t required)
{
  size_t new_size;

  if (buf->memsize>=required+1)
    return;

  new_size = size_enough(buf->memsize, required+1);
  
  /* FIXME: Realloc? Problems? */
  buf->c_str = (char *)realloc(buf->c_str, new_size);
  buf->memsize = new_size;
}

Buffer *buffer_alloc()
{
  /* Alloc handle */
  Buffer *buf = (Buffer *)malloc(sizeof(Buffer));
  
  /* Alloc contents */
  buf->memsize = BUF_ALLOC_BASE;
  buf->c_str = (char *)malloc(buf->memsize);

  buffer_clear(buf);

  return buf;
}

void buffer_free(Buffer *buf)
{
  free(buf->c_str);
  free(buf);
}

void buffer_clear(Buffer *buf)
{
  buf->length = 0;
  buf->c_str[0] = '\0';
}

void buffer_putchar(Buffer *buf, char c)
{
  const size_t length = buf->length;
  buffer_ensure_size(buf, length+1);

  buf->c_str[length] = c;
  buf->c_str[length+1] = '\0';
  buf->length++;
}

void buffer_copy(Buffer *dest, const char *src)
{
  const int length = strlen(src);
  buffer_ensure_size(dest, length+1);

  dest->length = length;
  memcpy(dest->c_str, src, length);
  dest->c_str[length] = '\0';
}

char *buffer_end(Buffer *buf)
{
  return buf->c_str + buf->length;
}
