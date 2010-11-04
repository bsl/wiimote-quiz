#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "button_event.h"
#include "controller.h"
#include "debug.h"
#include "ending.h"
#include "rqueue.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

static void handle_button(struct button_event *b);

/* - - - - - - - - - - - - - - - - - - - - */

void *
controller_run(void *v)
{
  struct controller_run_args *args;
  rqueue_t buttonsq, commandsq;
  ending_t ending;
  struct button_event *b;

  args = v;

  ending    = args->ending;
  buttonsq  = args->buttonsq;
  commandsq = args->commandsq;

  while (1) {
    while (!rqueue_is_empty(buttonsq)) {
      b = rqueue_remove(buttonsq);
      handle_button(b);
      free(b);
    }

    if (ending_get(ending)) {
      break;
    }

    usleep(10000);
  }

  return NULL;
}

/* - - - - - - - - - - - - - - - - - - - - */

static void
handle_button(struct button_event *b)
{
  print_info("button press event: id=%d, button=%d", b->id, b->button);
}
