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
#include "min_heap.h"
#include "queue.h"
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
static void handle_highlevel_wiimote_command(struct highlevel_wiimote_command *c, min_heap_t lowlevel_commandsq);

/* - - - - - - - - - - - - - - - - - - - - */

void *
wiimotes_run(void *v)
{
  struct wiimotes_run_args *args;
  wiimotes_t w;
  rqueue_t buttonsq, highlevel_commandsq;
  min_heap_t lowlevel_commandsq;
  int i, num_events;
  unsigned int max_num_wiimotes;
  unsigned int find_time_in_sec;
  ending_t ending;

  args = v;

  ending              = args->ending;
  max_num_wiimotes    = args->max_num_wiimotes;
  find_time_in_sec    = args->max_num_wiimotes;
  buttonsq            = args->buttonsq;
  highlevel_commandsq = args->commandsq;

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
    while (!rqueue_is_empty(highlevel_commandsq)) {
      struct highlevel_wiimote_command *c = rqueue_remove(highlevel_commandsq);
      handle_highlevel_wiimote_command(c, lowlevel_commandsq);
    }

    /* Send low-level commands whose times have arrived to the wiimotes.
     */
    // ...

    usleep(10000);

    if (ending_get(ending)) {
      break;
    }
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
handle_highlevel_wiimote_command(struct highlevel_wiimote_command *c, min_heap_t lowlevel_commandsq)
{
  (void)c;
  (void)lowlevel_commandsq;
}
