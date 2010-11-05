#include <stdio.h>
#include <stdlib.h>

#include "lowlevel_command.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct lowlevel_command *
lowlevel_command_new(void)
{
  struct lowlevel_command *llc;

  llc = malloc(sizeof(*llc));
  llc->type = LOWLEVEL_COMMAND_UNKNOWN;

  return llc;
}

void
lowlevel_command_free(struct lowlevel_command *llc)
{
  free(llc);
}
