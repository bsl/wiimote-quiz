#define _XOPEN_SOURCE 500 /* usleep */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <wiiuse.h>

#include "controller.h"
#include "debug.h"
#include "display.h"
#include "ending.h"
#include "main.h"
#include "rqueue.h"
#include "timer.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

static int install_SIGINT_handler(void);
static void handle_sigint(int);

/* - - - - - - - - - - - - - - - - - - - - */

struct {
  ending_t ending;
  rqueue_t buttonsq, hlcommandsq;
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

  /* start display thread */
  d_args.ending           = g.ending;
  pthread_create(&g.threads[2], NULL, display_run, &d_args);

  if (!install_SIGINT_handler()) {
    return EXIT_FAILURE;
  }

  if (!timer_init()) {
    return EXIT_FAILURE;
  }

  while (1) {
    unsigned long ms;

    sleep(5);

    if (timer_get_elapsed_ms(&ms)) {
      print_info("%ld ms elapsed", ms);
    }
  }

  return EXIT_SUCCESS;
}

/* - - - - - - - - - - - - - - - - - - - - */

static int
install_SIGINT_handler(void)
{
  print_info("installing SIGINT handler");
  if (signal(SIGINT, handle_sigint) == SIG_ERR) {
    print_info("signal: %s", strerror(errno));
    return 0;
  }

  print_info("SIGINT handler installed");
  return 1;
}

static void
handle_sigint(int signum)
{
  int i, num_threads;

  (void)signum;

  print_info("SIGINT handler cleaning up");

  /* tell all threads to end */
  ending_set(g.ending, true);

  num_threads = sizeof(g.threads) / sizeof(pthread_t);
  print_info("num_threads = %d", num_threads);
  for (i=0; i<num_threads; i++) {
    print_info("joining thread %d", i);
    pthread_join(g.threads[i], NULL);
  }

  ending_free(g.ending);
  rqueue_free(g.buttonsq);
  rqueue_free(g.hlcommandsq);

  print_info("SIGINT handler exiting");
  exit(EXIT_SUCCESS);
}
