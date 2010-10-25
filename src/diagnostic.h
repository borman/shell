#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

typedef struct
{
  int error;                 /* error flag */
  const char *error_message; /* error description */
} Diagnostic;

#endif /* DIAGNOSTIC_H */

