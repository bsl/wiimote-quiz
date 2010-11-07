#ifndef __CONTROLLER_STATE_H__
#define __CONTROLLER_STATE_H__

#include "main.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct controller_state_s {
  char question[128];
  unsigned int num_buzzed;
  int buzzed[WIIMOTES_MAX_NUM];
  unsigned long last_updated;
};

typedef struct controller_state_s2 * controller_state_t;

/* - - - - - - - - - - - - - - - - - - - - */

controller_state_t controller_state_new  (                    struct controller_state_s *);
void               controller_state_get  (controller_state_t, struct controller_state_s *);
void               controller_state_set  (controller_state_t, struct controller_state_s *);
void               controller_state_free (controller_state_t);

#endif
