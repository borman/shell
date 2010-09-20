#include "readline.h"

int main(int argc, char **argv)
{
  Buffer *linebuffer = buffer_alloc();
  READLINE_RESULT result;
  
  do
  {
    result = readline(NULL);
/*    CommandNode * */
  } while (result != READLINE_EOF);

  buffer_free(linebuffer);

  return 0;
}

