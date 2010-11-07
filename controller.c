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
static void flash_leds(int wiimote_num, rqueue_t hlcommandsq);
static void rumble(int wiimote_num, rqueue_t hlcommandsq);

/* - - - - - - - - - - - - - - - - - - - - */

void *
controller_run(void *v)
{
  struct controller_run_args *c_args = v;
  ending_t ending                    = c_args->ending;
  rqueue_t buttonsq                  = c_args->buttonsq;
  rqueue_t hlcommandsq               = c_args->hlcommandsq;

  struct button_event *b;

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
  const char *button_name;

  switch (b->button) {
    case WIIMOTE_BUTTON_A:     button_name = "A";         break;
    case WIIMOTE_BUTTON_B:     button_name = "B";         break;
    case WIIMOTE_BUTTON_UP:    button_name = "UP";        break;
    case WIIMOTE_BUTTON_DOWN:  button_name = "DOWN";      break;
    case WIIMOTE_BUTTON_LEFT:  button_name = "LEFT";      break;
    case WIIMOTE_BUTTON_RIGHT: button_name = "RIGHT";     break;
    case WIIMOTE_BUTTON_MINUS: button_name = "MINUS";     break;
    case WIIMOTE_BUTTON_PLUS:  button_name = "PLUS";      break;
    case WIIMOTE_BUTTON_ONE:   button_name = "ONE";       break;
    case WIIMOTE_BUTTON_TWO:   button_name = "TWO";       break;
    case WIIMOTE_BUTTON_HOME:  button_name = "HOME";      break;
    default:                   button_name = "<unknown>"; break;
  }

  print_info("button pressed: wiimote %d, button %s", b->id, button_name);

  switch (b->button) {
    case WIIMOTE_BUTTON_A: flash_leds(b->id, hlcommandsq); break;
    case WIIMOTE_BUTTON_B: rumble(b->id, hlcommandsq);     break;
  }
}

static void flash_leds(int wiimote_num, rqueue_t hlcommandsq)
{
  struct highlevel_command *hlc;

  hlc = highlevel_command_new(wiimote_num, HIGHLEVEL_COMMAND_FLASH);
  hlc->parameters.flash.pattern_num = 0;
  rqueue_add(hlcommandsq, hlc);
}

static void rumble(int wiimote_num, rqueue_t hlcommandsq)
{
  struct highlevel_command *hlc;

  hlc = highlevel_command_new(wiimote_num, HIGHLEVEL_COMMAND_RUMBLE);
  hlc->parameters.rumble.pattern_num = 0;
  rqueue_add(hlcommandsq, hlc);
}
