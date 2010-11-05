#include <stdio.h>
#include <stdlib.h>

#include "lowlevel_command.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct lowlevel_command *
lowlevel_command_new(int wiimote_num, enum lowlevel_command_type type)
{
  struct lowlevel_command *llc;

  llc = malloc(sizeof(*llc));
  llc->wiimote_num = wiimote_num;
  llc->type = type;

  return llc;
}

void
lowlevel_command_free(struct lowlevel_command *llc)
{
  free(llc);
}
