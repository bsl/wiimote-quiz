#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "controller_state.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct controller_state_s2 {
  struct controller_state_s *s;
  pthread_mutex_t m;
};

/* - - - - - - - - - - - - - - - - - - - - */

controller_state_t
controller_state_new(struct controller_state_s *s)
{
  controller_state_t cs;

  cs = malloc(sizeof(*cs));
  cs->s = malloc(sizeof(struct controller_state_s));
  memcpy(cs->s, s, sizeof(struct controller_state_s));
  pthread_mutex_init(&cs->m, NULL);

  return cs;
}

void
controller_state_get(controller_state_t cs, struct controller_state_s *s)
{
  pthread_mutex_lock(&cs->m);
  memcpy(s, cs->s, sizeof(struct controller_state_s));
  pthread_mutex_unlock(&cs->m);
}

void
controller_state_set(controller_state_t cs, struct controller_state_s *s)
{
  pthread_mutex_lock(&cs->m);
  memcpy(cs->s, s, sizeof(struct controller_state_s));
  pthread_mutex_unlock(&cs->m);
}

void
controller_state_free(controller_state_t cs)
{
  free(cs->s);
  pthread_mutex_destroy(&cs->m);
  free(cs);
}
