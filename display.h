#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct display_run_args {
  ending_t ending;
  /* XXX controller state */
};

/* - - - - - - - - - - - - - - - - - - - - */

void *display_run(void *);

#endif
