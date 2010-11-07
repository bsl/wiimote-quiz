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
  struct display_run_args *d_args = v;
  ending_t ending                 = d_args->ending;
  controller_state_t cs           = d_args->cs;

  (void)cs;

  while (1) {
    if (ending_get(ending)) {
      break;
    }

    usleep(10000);
  }

  return NULL;
}
