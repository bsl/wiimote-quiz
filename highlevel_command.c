#include <stdio.h>
#include <stdlib.h>

#include "highlevel_command.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct highlevel_command *
highlevel_command_new(void)
{
  struct highlevel_command *hlc;

  hlc = malloc(sizeof(*hlc));
  hlc->type = HIGHLEVEL_COMMAND_UNKNOWN;

  return hlc;
}

void
highlevel_command_free(struct highlevel_command *hlc)
{
  free(hlc);
}
