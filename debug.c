#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "debug.h"

/* - - - - - - - - - - - - - - - - - - - - */

void
print_info(const char *format, ...)
{
  va_list ap;
  char buf[256];

  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  fprintf(stderr, ">>> %s\n", buf);
  fflush(stderr);
}
