#include <stdio.h>
#include <stdlib.h>

#include "highlevel_wiimote_command.h"

struct highlevel_wiimote_command *
highlevel_wiimote_command_new(void)
{
  struct highlevel_wiimote_command *hlc;

  hlc = malloc(sizeof(*hlc));
  hlc->type = HIGHLEVEL_WIIMOTE_COMMAND_UNKNOWN;

  return hlc;
}

void
highlevel_wiimote_command_free(struct highlevel_wiimote_command *hlc)
{
  free(hlc);
}
