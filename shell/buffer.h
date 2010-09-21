#ifndef BUFFER_H
#define BUFFER_H

#include <sys/types.h>

typedef struct
{
  /* public */
  char *c_str;
  
  /* private */
  size_t memsize;
  size_t length;
} Buffer;

Buffer *buffer_alloc();
void buffer_free(Buffer *buffer);

void buffer_clear(Buffer *buffer);
void buffer_putchar(Buffer *buffer, char c);
void buffer_copy(Buffer *dest, const char *src);

char *buffer_end(Buffer *buf);


#endif


