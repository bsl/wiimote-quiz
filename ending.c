#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>

#include "ending.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct ending_s {
  bool ending;
  pthread_mutex_t m;
};

/* - - - - - - - - - - - - - - - - - - - - */

ending_t
ending_new(bool b)
{
  ending_t e;

  e = malloc(sizeof(*e));
  e->ending = b;
  pthread_mutex_init(&e->m, NULL);

  return e;
}

bool
ending_get(ending_t e)
{
  bool b;

  pthread_mutex_lock(&e->m);
  b = e->ending;
  pthread_mutex_unlock(&e->m);

  return b;
}

bool
ending_set(ending_t e, bool b2)
{
  bool b;

  pthread_mutex_lock(&e->m);
  b         = e->ending;
  e->ending = b2;
  pthread_mutex_unlock(&e->m);

  return b;
}

void
ending_free(ending_t e)
{
  pthread_mutex_destroy(&e->m);
  free(e);
}
