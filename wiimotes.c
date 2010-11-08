#define _XOPEN_SOURCE 500  /* usleep */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <wiiuse.h>

#include "button_event.h"
#include "debug.h"
#include "ending.h"
#include "highlevel_command.h"
#include "lowlevel_command.h"
#include "llcmdqueue.h"
#include "rqueue.h"
#include "timer.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct wiimotes {
  wiimote **wiimotes;
  int size;
};

/* - - - - - - - - - - - - - - - - - - - - */

static wiimotes_t init(unsigned int m);
static void deinit(wiimotes_t w);
static void associate(wiimotes_t w, unsigned int find_time_in_sec, llcmdqueue_t llcommandsq);
static void set_leds_by_wiimote_num(int num, llcmdqueue_t llcommandsq, unsigned long when);
static void set_leds_by_bits(int num, llcmdqueue_t llcommandsq, unsigned long when, int bits);
static int calc_leds_by_bits(int);
static void handle_button_event(wiimotes_t w, int n, rqueue_t buttonsq);
static void add_button_event(rqueue_t rq, int id, int button);
static void handle_highlevel_command(const struct highlevel_command *hlc, llcmdqueue_t llcommandsq);
static void handle_lowlevel_command(const struct lowlevel_command *llc, wiimotes_t w);

/* - - - - - - - - - - - - - - - - - - - - */

void *
wiimotes_run(void *v)
{
  struct wiimotes_run_args *w_args = v;
  ending_t ending                  = w_args->ending;
  unsigned int max_num_wiimotes    = w_args->max_num_wiimotes;
  unsigned int find_time_in_sec    = w_args->find_time_in_sec;
  rqueue_t buttonsq                = w_args->buttonsq;
  rqueue_t hlcommandsq             = w_args->hlcommandsq;

  wiimotes_t w;
  llcmdqueue_t llcommandsq;
  struct highlevel_command *hlc;
  struct lowlevel_command *llc;
  int i, num_events;
  unsigned long t, when_to_send;

  /* Initialize command queue
   */
  llcommandsq = llcmdqueue_new();

  /* Initialize wiiuse and associate with wiimotes.
   */
  w = init(max_num_wiimotes);
  associate(w, find_time_in_sec, llcommandsq);

  while (1) {
    /* Relay incoming button events from the wiimotes to the controller via the
     * buttons queue.
     */
    num_events = wiiuse_poll(w->wiimotes, w->size);
    for (i=0; num_events>0 && i<w->size; i++) {
      if (w->wiimotes[i]->event == WIIUSE_EVENT) {
        handle_button_event(w, i, buttonsq);
        num_events--;
      }
    }

    /* Translate high-level commands from the controller to low-level commands
     * to be sent to the wiimotes.
     */
    while (!rqueue_is_empty(hlcommandsq)) {
      hlc = rqueue_remove(hlcommandsq);
      handle_highlevel_command(hlc, llcommandsq);
      highlevel_command_free(hlc);
    }

    /* Send low-level commands whose times have arrived to the wiimotes.
     */
    timer_get_elapsed_ms(&t);
    while (!llcmdqueue_is_empty(llcommandsq)) {
      when_to_send = llcmdqueue_peek_key(llcommandsq);
      if (when_to_send <= t) {
        llc = llcmdqueue_remove(llcommandsq);
        handle_lowlevel_command(llc, w);
        lowlevel_command_free(llc);
      } else {
        break;
      }
    }

    if (ending_get(ending)) {
      break;
    }

    usleep(10000);
  }

  while (!llcmdqueue_is_empty(llcommandsq)) {
    llc = llcmdqueue_remove(llcommandsq);
    lowlevel_command_free(llc);
  }
  llcmdqueue_free(llcommandsq);

  deinit(w);

  print_info("wiimotes exiting");

  return NULL;
}

/* - - - - - - - - - - - - - - - - - - - - */

static wiimotes_t
init(unsigned int m)
{
  wiimotes_t w = malloc(sizeof(*w));

  w->wiimotes = wiiuse_init((int)m);
  w->size     = m;

  return w;
}

static void
deinit(wiimotes_t w)
{
  wiiuse_cleanup(w->wiimotes, w->size);
  free(w);
}

static void
associate(wiimotes_t w, unsigned int find_time_in_sec, llcmdqueue_t llcommandsq)
{
  unsigned long t;

  wiiuse_find(w->wiimotes, w->size, find_time_in_sec);
  wiiuse_connect(w->wiimotes, w->size);

  timer_get_elapsed_ms(&t);
  for (int i=1; i<=w->size; i++) {
    set_leds_by_wiimote_num(i, llcommandsq, t);
  }
}

static void
set_leds_by_wiimote_num(int wiimote_num, llcmdqueue_t llcommandsq, unsigned long when)
{
  set_leds_by_bits(wiimote_num, llcommandsq, when, wiimote_num);
}

static void
set_leds_by_bits(int wiimote_num, llcmdqueue_t llcommandsq, unsigned long when, int bits)
{
  struct lowlevel_command *llc;

  llc = lowlevel_command_new(wiimote_num, LOWLEVEL_COMMAND_SET_LEDS);
  llc->parameters.set_leds.leds = calc_leds_by_bits(bits);

  llcmdqueue_add(llcommandsq, llc, when);
}

static int
calc_leds_by_bits(int bits)
{
  int leds;

  leds = 0;

  if (bits & 1) leds |= WIIMOTE_LED_4;
  if (bits & 2) leds |= WIIMOTE_LED_3;
  if (bits & 4) leds |= WIIMOTE_LED_2;
  if (bits & 8) leds |= WIIMOTE_LED_1;

  return leds;
}

static void
handle_button_event(wiimotes_t w, int n, rqueue_t buttonsq)
{
  struct wiimote_t *wm;
  int id;

  wm = w->wiimotes[n];
  id = wm->unid;

  if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))     add_button_event(buttonsq, id, WIIMOTE_BUTTON_A);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))     add_button_event(buttonsq, id, WIIMOTE_BUTTON_B);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))    add_button_event(buttonsq, id, WIIMOTE_BUTTON_UP);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))  add_button_event(buttonsq, id, WIIMOTE_BUTTON_DOWN);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))  add_button_event(buttonsq, id, WIIMOTE_BUTTON_LEFT);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)) add_button_event(buttonsq, id, WIIMOTE_BUTTON_RIGHT);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) add_button_event(buttonsq, id, WIIMOTE_BUTTON_MINUS);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))  add_button_event(buttonsq, id, WIIMOTE_BUTTON_PLUS);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))   add_button_event(buttonsq, id, WIIMOTE_BUTTON_ONE);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))   add_button_event(buttonsq, id, WIIMOTE_BUTTON_TWO);
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))  add_button_event(buttonsq, id, WIIMOTE_BUTTON_HOME);
}

static void
add_button_event(rqueue_t buttonsq, int id, int button)
{
  rqueue_add(buttonsq, button_event_new(id, button));
}

static void
handle_highlevel_command(const struct highlevel_command *hlc, llcmdqueue_t llcommandsq)
{
  struct lowlevel_command *llc;
  unsigned long t;

  switch (hlc->type) {
    case HIGHLEVEL_COMMAND_FLASH:  /* XXX ignoring pattern_num parameter for now */
      timer_get_elapsed_ms(&t);
      int all_leds_on = WIIMOTE_LED_1 | WIIMOTE_LED_2 | WIIMOTE_LED_3 | WIIMOTE_LED_4;

      set_leds_by_bits(hlc->wiimote_num, llcommandsq, t,     0);           /* turn all leds off */
      set_leds_by_bits(hlc->wiimote_num, llcommandsq, t+100, all_leds_on); /* turn all leds on  */
      set_leds_by_bits(hlc->wiimote_num, llcommandsq, t+200, 0);           /* turn all leds off */
      set_leds_by_bits(hlc->wiimote_num, llcommandsq, t+300, all_leds_on); /* turn all leds on  */
      set_leds_by_bits(hlc->wiimote_num, llcommandsq, t+400, 0);           /* turn all leds off */
      set_leds_by_wiimote_num(hlc->wiimote_num, llcommandsq, t+500);       /* set it back */

      break;

    case HIGHLEVEL_COMMAND_RUMBLE:  /* XXX ignoring pattern_num parameter for now */
      timer_get_elapsed_ms(&t);

      /* turn rumble on */
      llc                           = lowlevel_command_new(hlc->wiimote_num, LOWLEVEL_COMMAND_RUMBLE);
      llc->parameters.rumble.active = true;
      llcmdqueue_add(llcommandsq, llc, t);

      /* turn rumble off */
      llc                           = lowlevel_command_new(hlc->wiimote_num, LOWLEVEL_COMMAND_RUMBLE);
      llc->parameters.rumble.active = false;
      llcmdqueue_add(llcommandsq, llc, t+400);

      /* XXX turn rumble off again for safety */
      llc                           = lowlevel_command_new(hlc->wiimote_num, LOWLEVEL_COMMAND_RUMBLE);
      llc->parameters.rumble.active = false;
      llcmdqueue_add(llcommandsq, llc, t+500);

      break;

    case HIGHLEVEL_COMMAND_UNKNOWN:
      break;
  }
}

static void
handle_lowlevel_command(const struct lowlevel_command *llc, wiimotes_t w)
{
  int wmidx = llc->wiimote_num - 1;

  if (wmidx < 0 || wmidx >= w->size) {
    print_info("bad wiimote index: %d  w->size:%d", wmidx, w->size);
  } else {
    struct wiimote_t *wm = w->wiimotes[wmidx];

    switch (llc->type) {
      case LOWLEVEL_COMMAND_SET_LEDS:
        wiiuse_set_leds(wm, llc->parameters.set_leds.leds);
        break;

      case LOWLEVEL_COMMAND_RUMBLE:
        wiiuse_rumble(wm, llc->parameters.rumble.active ? 1 : 0);
        break;

      case LOWLEVEL_COMMAND_UNKNOWN:
        break;
    }
  }
}
