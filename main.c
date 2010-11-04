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
#include "ending.h"
#include "rqueue.h"
#include "wiimotes.h"

/* - - - - - - - - - - - - - - - - - - - - */

#define WIIMOTES_MAX_NUM          15
#define WIIMOTES_FIND_TIME_IN_SEC 4

/* - - - - - - - - - - - - - - - - - - - - */

static int install_SIGINT_handler(void);
static void handle_sigint(int);

/* - - - - - - - - - - - - - - - - - - - - */

struct {
  ending_t ending;
  rqueue_t buttonsq, commandsq;
  pthread_t threads[2];
} g;

/* - - - - - - - - - - - - - - - - - - - - */

int
main(int argc, char **argv)
{
  struct wiimotes_run_args   w_args;
  struct controller_run_args c_args;

  (void)argc;
  (void)argv;

  g.ending    = ending_new(false);
  g.buttonsq  = rqueue_new();
  g.commandsq = rqueue_new();

  /* start controller thread */
  c_args.ending    = g.ending;
  c_args.buttonsq  = g.buttonsq;
  c_args.commandsq = g.commandsq;
  pthread_create(&g.threads[1], NULL, controller_run, &c_args);

  /* start wiimotes thread */
  w_args.ending           = g.ending;
  w_args.max_num_wiimotes = WIIMOTES_MAX_NUM;
  w_args.find_time_in_sec = WIIMOTES_FIND_TIME_IN_SEC;
  w_args.buttonsq         = g.buttonsq;
  pthread_create(&g.threads[0], NULL, wiimotes_run, &w_args);

  if (!install_SIGINT_handler()) {
    return EXIT_FAILURE;
  }

  while (1) {
    sleep(30);
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
  rqueue_free(g.commandsq);

  print_info("SIGINT handler exiting");
  exit(EXIT_SUCCESS);
}
