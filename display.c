#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <unistd.h>

#include "controller.h"
#include "controller_state.h"
#include "display.h"
#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

void *
display_run(void *v)
{
  struct display_run_args *args;
  ending_t ending;
  controller_state_t cs;

  args = v;

  ending = args->ending;
  cs     = args->cs;

  while (1) {
    if (ending_get(ending)) {
      break;
    }

    usleep(10000);
  }

  return NULL;
}
