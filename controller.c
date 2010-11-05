#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <wiiuse.h> // XXX

#include "button_event.h"
#include "controller.h"
#include "debug.h"
#include "ending.h"
#include "highlevel_command.h"
#include "rqueue.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

static void handle_button(struct button_event *b, rqueue_t hlcommandsq);
static void flash_leds(struct button_event *b, rqueue_t hlcommandsq);
static void rumble(struct button_event *b, rqueue_t hlcommandsq);

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
      button_event_free(b);
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
  switch (b->button) {
    case WIIMOTE_BUTTON_A:
      print_info("button press event: id=%d, button=A", b->id);
      flash_leds(b, hlcommandsq);
      break;
    case WIIMOTE_BUTTON_B:
      print_info("button press event: id=%d, button=B", b->id);
      break;
    case WIIMOTE_BUTTON_UP:
      print_info("button press event: id=%d, button=UP", b->id);
      rumble(b, hlcommandsq);
      break;
    case WIIMOTE_BUTTON_DOWN:
      print_info("button press event: id=%d, button=DOWN", b->id);
      rumble(b, hlcommandsq);
      break;
    case WIIMOTE_BUTTON_LEFT:
      print_info("button press event: id=%d, button=LEFT", b->id);
      rumble(b, hlcommandsq);
      break;
    case WIIMOTE_BUTTON_RIGHT:
      print_info("button press event: id=%d, button=RIGHT", b->id);
      rumble(b, hlcommandsq);
      break;
    case WIIMOTE_BUTTON_MINUS:
      print_info("button press event: id=%d, button=MINUS", b->id);
      break;
    case WIIMOTE_BUTTON_PLUS:
      print_info("button press event: id=%d, button=PLUS", b->id);
      break;
    case WIIMOTE_BUTTON_ONE:
      print_info("button press event: id=%d, button=ONE", b->id);
      break;
    case WIIMOTE_BUTTON_TWO:
      print_info("button press event: id=%d, button=TWO", b->id);
      break;
    case WIIMOTE_BUTTON_HOME:
      print_info("button press event: id=%d, button=HOME", b->id);
      break;
    default:
      print_info("button press event: id=%d, button=%d", b->id, b->button);
      break;
  }
}

static void flash_leds(struct button_event *b, rqueue_t hlcommandsq)
{
  struct highlevel_command *hlc;

  hlc = highlevel_command_new(b->id, HIGHLEVEL_COMMAND_FLASH);
  hlc->parameters.flash.pattern_num = 0;
  rqueue_add(hlcommandsq, hlc);
}

static void rumble(struct button_event *b, rqueue_t hlcommandsq)
{
  struct highlevel_command *hlc;

  hlc = highlevel_command_new(b->id, HIGHLEVEL_COMMAND_RUMBLE);
  hlc->parameters.rumble.pattern_num = 0;
  rqueue_add(hlcommandsq, hlc);
}
