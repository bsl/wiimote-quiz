#define _POSIX_C_SOURCE 199309L /* clock_gettime */

#include <string.h>
#include <errno.h>
#include <time.h>

#include <pthread.h>

#include "debug.h"
#include "timer.h"

/* - - - - - - - - - - - - - - - - - - - - */

#define MSEC_PER_SEC  1E3
#define USEC_PER_SEC  1E9
#define MSEC_PER_USEC 1E6

/* - - - - - - - - - - - - - - - - - - - - */

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static struct timespec init_time;

/* - - - - - - - - - - - - - - - - - - - - */

static void timespec_diff(const struct timespec *const start, const struct timespec *const end, struct timespec *r);
static unsigned long get_ms_diff(struct timespec *ts1, struct timespec *ts2);

/* - - - - - - - - - - - - - - - - - - - - */

int
timer_init(void)
{
  int status;

  pthread_mutex_lock(&mtx);
  status = clock_gettime(CLOCK_MONOTONIC, &init_time);
  pthread_mutex_unlock(&mtx);

  if (status == -1) {
    print_info("clock_gettime: %s", strerror(errno));
  }

  return status == 0;
}

int
timer_elapsed(unsigned long *ms)
{
  struct timespec tp;
  int status;

  status = clock_gettime(CLOCK_MONOTONIC, &tp);
  if (status == -1) {
    print_info("clock_gettime: %s", strerror(errno));
  } else if (status == 0) {
    *ms = get_ms_diff(&init_time, &tp);
  }

  return status == 0;
}

/* - - - - - - - - - - - - - - - - - - - - */

static void
timespec_diff(const struct timespec *const start, const struct timespec *const end, struct timespec *r)
{
  if ((end->tv_nsec - start->tv_nsec) < 0) {
    r->tv_sec = end->tv_sec-start->tv_sec - 1;
    r->tv_nsec = USEC_PER_SEC + end->tv_nsec-start->tv_nsec;
  } else {
    r->tv_sec = end->tv_sec-start->tv_sec;
    r->tv_nsec = end->tv_nsec-start->tv_nsec;
  }
}

static unsigned long
get_ms_diff(struct timespec *ts1, struct timespec *ts2)
{
  unsigned long ms;
  struct timespec diff;

  timespec_diff(ts1, ts2, &diff);

  ms = diff.tv_sec*MSEC_PER_SEC + diff.tv_nsec/MSEC_PER_USEC;

  return ms;
}
