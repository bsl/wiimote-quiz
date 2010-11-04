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
#include "ending.h"
#include "highlevel_wiimote_command.h"
#include "lowlevel_wiimote_command.h"
#include "min_heap.h"
#include "queue.h"
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
static void associate(wiimotes_t w, unsigned int find_time_in_sec);
static void set_all_numbers(wiimotes_t w);
static void set_number(wiimotes_t w, int n);
static void handle_button_event(wiimotes_t w, int n, rqueue_t buttonsq);
static void add_button_event(rqueue_t rq, int id, int button);
static void handle_highlevel_wiimote_command(struct highlevel_wiimote_command *hlc, min_heap_t llcommandsq);
static void insert_lowlevel_wiimote_command(const struct lowlevel_wiimote_command *llc, min_heap_t llcommandsq);
static void handle_lowlevel_wiimote_command(const struct lowlevel_wiimote_command *llc, wiimotes_t w);

/* - - - - - - - - - - - - - - - - - - - - */

void *
wiimotes_run(void *v)
{
  struct wiimotes_run_args *args;
  wiimotes_t w;
  rqueue_t buttonsq, hlcommandsq;
  min_heap_t llcommandsq;
  int i, num_events;
  unsigned int max_num_wiimotes;
  unsigned int find_time_in_sec;
  ending_t ending;
  unsigned long t;
  struct highlevel_wiimote_command *hlc;
  struct lowlevel_wiimote_command *llc;

  args = v;
  ending           = args->ending;
  max_num_wiimotes = args->max_num_wiimotes;
  find_time_in_sec = args->max_num_wiimotes;
  buttonsq         = args->buttonsq;
  hlcommandsq     = args->commandsq;

  /* Initialize wiiuse and associate with wiimotes.
   */
  w = init(max_num_wiimotes);
  associate(w, find_time_in_sec);

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
      handle_highlevel_wiimote_command(hlc, llcommandsq);
      free(hlc);
    }

    /* Send low-level commands whose times have arrived to the wiimotes.
     */
    timer_get_elapsed_ms(&t);
    while (1) {
      llc = min_heap_peek(llcommandsq);
      if (llc->when_to_send >= t) {
        (void)min_heap_remove(llcommandsq);
        handle_lowlevel_wiimote_command(llc, w);
        free(llc);
      } else {
        break;
      }
    }

    if (ending_get(ending)) {
      break;
    }

    usleep(10000);
  }

  deinit(w);

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
associate(wiimotes_t w, unsigned int find_time_in_sec)
{
  wiiuse_find(w->wiimotes, w->size, find_time_in_sec);
  wiiuse_connect(w->wiimotes, w->size);

  set_all_numbers(w);
}

static void
set_all_numbers(wiimotes_t w)
{
  int n;

  for (n=0; n<w->size; n++) {
    set_number(w, n);
  }
}

static void
set_number(wiimotes_t w, int n)
{
  int leds, p;

  leds = 0;
  p = n + 1;

  if (p & 1) leds |= WIIMOTE_LED_4;
  if (p & 2) leds |= WIIMOTE_LED_3;
  if (p & 4) leds |= WIIMOTE_LED_2;
  if (p & 8) leds |= WIIMOTE_LED_1;

  wiiuse_set_leds(w->wiimotes[n], leds);
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
  struct button_event *b;

  b = malloc(sizeof(*b));

  b->id     = id;
  b->button = button;

  rqueue_add(buttonsq, b);
}

static void
handle_highlevel_wiimote_command(struct highlevel_wiimote_command *hlc, min_heap_t llcommandsq)
{
  struct lowlevel_wiimote_command llc;
  unsigned long t;

  switch (hlc->type) {
    case HIGHLEVEL_WIIMOTE_COMMAND_FLASH:
      break;

    case HIGHLEVEL_WIIMOTE_COMMAND_RUMBLE:  /* XXX ignoring pattern_num parameter for now */
      timer_get_elapsed_ms(&t);

      llc.wiimote_num = hlc->wiimote_num;
      llc.type        = LOWLEVEL_WIIMOTE_COMMAND_RUMBLE;

      /* turn rumble on */
      llc.when_to_send             = t;
      llc.parameters.rumble.active = true;
      insert_lowlevel_wiimote_command(&llc, llcommandsq);

      /* turn rumble off */
      llc.when_to_send             = t + 1000;
      llc.parameters.rumble.active = false;
      insert_lowlevel_wiimote_command(&llc, llcommandsq);

      break;
  }
}

static void
insert_lowlevel_wiimote_command(const struct lowlevel_wiimote_command *llc, min_heap_t llcommandsq)
{
  struct lowlevel_wiimote_command *d;

  d = malloc(sizeof(*d));
  memcpy(d, llc, sizeof(*d));

  min_heap_insert(llcommandsq, (int)d->when_to_send, d);
}

static void
handle_lowlevel_wiimote_command(const struct lowlevel_wiimote_command *llc, wiimotes_t w)
{
  switch (llc->type) {
    case LOWLEVEL_WIIMOTE_COMMAND_SET_LEDS:
      wiiuse_set_leds(w->wiimotes[llc->wiimote_num], llc->parameters.set_leds.leds);
      break;

    case LOWLEVEL_WIIMOTE_COMMAND_RUMBLE:
      wiiuse_rumble(w->wiimotes[llc->wiimote_num], llc->parameters.rumble.active ? 1 : 0);
      break;
  }
}
