#define _POSIX_C_SOURCE 199309L /* clock_gettime */

#include <string.h>
#include <errno.h>
#include <time.h>

#include <pthread.h>

#include "debug.h"
#include "timer.h"

/* - - - - - - - - - - - - - - - - - - - - */

#define MS_PER_S  1E3
#define US_PER_S  1E9
#define MS_PER_US 1E6

/* - - - - - - - - - - - - - - - - - - - - */

static struct timespec init_time;
static pthread_mutex_t init_time_mtx = PTHREAD_MUTEX_INITIALIZER;

/* - - - - - - - - - - - - - - - - - - - - */

static unsigned long get_ms_diff(struct timespec *ts);
static void timespec_diff(const struct timespec *const start, const struct timespec *const end, struct timespec *r);

/* - - - - - - - - - - - - - - - - - - - - */

int
timer_init(void)
{
  int status;

  pthread_mutex_lock(&init_time_mtx);
  status = clock_gettime(CLOCK_MONOTONIC, &init_time);
  pthread_mutex_unlock(&init_time_mtx);

  if (status == -1) {
    print_info("clock_gettime: %s", strerror(errno));
  }

  return status == 0;
}

int
timer_get_elapsed_ms(unsigned long *ms)
{
  struct timespec ts;
  int status;

  status = clock_gettime(CLOCK_MONOTONIC, &ts);
  if (status == -1) {
    print_info("clock_gettime: %s", strerror(errno));
  } else if (status == 0) {
    *ms = get_ms_diff(&ts);
  }

  return status == 0;
}

/* - - - - - - - - - - - - - - - - - - - - */

static unsigned long
get_ms_diff(struct timespec *ts)
{
  unsigned long ms;
  struct timespec diff;

  pthread_mutex_lock(&init_time_mtx);
  timespec_diff(&init_time, ts, &diff);
  pthread_mutex_unlock(&init_time_mtx);

  ms = diff.tv_sec*MS_PER_S + diff.tv_nsec/MS_PER_US;

  return ms;
}

static void
timespec_diff(const struct timespec *const start, const struct timespec *const end, struct timespec *r)
{
  if ((end->tv_nsec - start->tv_nsec) < 0) {
    r->tv_sec = end->tv_sec-start->tv_sec - 1;
    r->tv_nsec = US_PER_S + end->tv_nsec-start->tv_nsec;
  } else {
    r->tv_sec = end->tv_sec-start->tv_sec;
    r->tv_nsec = end->tv_nsec-start->tv_nsec;
  }
}
