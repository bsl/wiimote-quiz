#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "controller_state.h"
#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct display_run_args {
  ending_t ending;
  controller_state_t cs;
};

/* - - - - - - - - - - - - - - - - - - - - */

void *display_run(void *);

#endif
