#include <stdio.h>
#include <stdlib.h>

#include "highlevel_command.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct highlevel_command *
highlevel_command_new(int wiimote_num, enum highlevel_command_type type)
{
  struct highlevel_command *hlc;

  hlc = malloc(sizeof(*hlc));
  hlc->wiimote_num = wiimote_num;
  hlc->type = type;

  return hlc;
}

void
highlevel_command_free(struct highlevel_command *hlc)
{
  free(hlc);
}
