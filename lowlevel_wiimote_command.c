#include <stdio.h>
#include <stdlib.h>

#include "lowlevel_wiimote_command.h"

struct lowlevel_wiimote_command *
lowlevel_wiimote_command_new(void)
{
  struct lowlevel_wiimote_command *llc;

  llc = malloc(sizeof(*llc));
  llc->type = LOWLEVEL_WIIMOTE_COMMAND_UNKNOWN;

  return llc;
}

void
lowlevel_wiimote_command_free(struct lowlevel_wiimote_command *llc)
{
  free(llc);
}
