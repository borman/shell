#include "buffer.h"
#include <stdlib.h>
#include <string.h>

const size_t BUF_ALLOC_BASE = 128;

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
  /* TODO: handle overflow */

  buf->c_str[length] = c;
  buf->c_str[length+1] = '\0';
  buf->length++;
}

void buffer_copy(Buffer *dest, const char *src)
{
  const int length = strlen(src);
  dest->length = length;

  /* TODO: handle overflow */
  strcpy(dest->c_str, src);
}

