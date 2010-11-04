#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "button_event.h"
#include "controller.h"
#include "debug.h"
#include "ending.h"
#include "highlevel_wiimote_command.h"
#include "rqueue.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

static void handle_button(struct button_event *b, rqueue_t hlcommandsq);

/* - - - - - - - - - - - - - - - - - - - - */

void *
controller_run(void *v)
{
  struct controller_run_args *args;
  rqueue_t buttonsq, hlcommandsq;
  ending_t ending;
  struct button_event *b;

  args = v;

  ending      = args->ending;
  buttonsq    = args->buttonsq;
  hlcommandsq = args->hlcommandsq;

  while (1) {
    while (!rqueue_is_empty(buttonsq)) {
      b = rqueue_remove(buttonsq);
      handle_button(b, hlcommandsq);
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
handle_button(struct button_event *b, rqueue_t hlcommandsq)
{
  struct highlevel_wiimote_command *hlc;

  print_info("button press event: id=%d, button=%d", b->id, b->button);

  /* flash leds in response to button press */
  hlc                               = highlevel_wiimote_command_new();
  hlc->wiimote_num                  = b->id;
  hlc->type                         = HIGHLEVEL_WIIMOTE_COMMAND_FLASH;
  hlc->parameters.flash.pattern_num = 0;

  rqueue_add(hlcommandsq, hlc);
}
