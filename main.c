#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <wiiuse.h>

#include "controller.h"
#include "controller_state.h"
#include "debug.h"
#include "display.h"
#include "ending.h"
#include "main.h"
#include "rqueue.h"
#include "timer.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

static struct {
  ending_t ending;
  rqueue_t buttonsq, hlcommandsq;
  controller_state_t cs;
  pthread_t threads[3];
} g;

/* - - - - - - - - - - - - - - - - - - - - */

int
main(int argc, char **argv)
{
  struct controller_run_args c_args;
  struct wiimotes_run_args   w_args;
  struct display_run_args    d_args;

  (void)argc;
  (void)argv;

  sigset_t sigs;
  sigfillset(&sigs);
  pthread_sigmask(SIG_BLOCK, &sigs, NULL);

  if (!timer_init()) {
    return EXIT_FAILURE;
  }

  g.ending      = ending_new(false);
  g.buttonsq    = rqueue_new();
  g.hlcommandsq = rqueue_new();

  /* start controller thread */
  c_args.ending      = g.ending;
  c_args.buttonsq    = g.buttonsq;
  c_args.hlcommandsq = g.hlcommandsq;
  pthread_create(&g.threads[0], NULL, controller_run, &c_args);

  /* start wiimotes thread */
  w_args.ending           = g.ending;
  w_args.max_num_wiimotes = WIIMOTES_MAX_NUM;
  w_args.find_time_in_sec = WIIMOTES_FIND_TIME_IN_SEC;
  w_args.buttonsq         = g.buttonsq;
  w_args.hlcommandsq      = g.hlcommandsq;
  pthread_create(&g.threads[1], NULL, wiimotes_run, &w_args);

  struct controller_state_s s;
  memset(&s, 0, sizeof(s));
  g.cs = controller_state_new(&s);

  /* start display thread */
  d_args.ending = g.ending;
  d_args.cs     = g.cs;
  pthread_create(&g.threads[2], NULL, display_run, &d_args);

  /* wait for ctrl-c */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  int sig;

  while (1) {
    sigwait(&sigs, &sig);
    if (sig == SIGINT) {
      ending_set(g.ending, true);

      int i;
      int num_threads = sizeof(g.threads) / sizeof(pthread_t);

      for (i=0; i<num_threads; i++) {
        pthread_join(g.threads[i], NULL);
      }

      ending_free(g.ending);
      rqueue_free(g.buttonsq);
      rqueue_free(g.hlcommandsq);
      controller_state_free(g.cs);

      break;
    }
  }

  print_info("main exiting");

  return EXIT_SUCCESS;
}
