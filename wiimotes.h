#ifndef __WIIMOTES_H__
#define __WIIMOTES_H__

#include "ending.h"
#include "rqueue.h"

typedef struct wiimotes * wiimotes_t;

struct wiimotes_run_args {
  ending_t ending;
  unsigned int max_num_wiimotes;
  unsigned int find_time_in_sec;
  rqueue_t buttonsq;
};

void *wiimotes_run(void *);

#endif
