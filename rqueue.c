#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "rqueue.h"
#include "queue.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct rqueue {
  queue_t q;
  pthread_mutex_t m;
};

/* - - - - - - - - - - - - - - - - - - - - */

rqueue_t
rqueue_new(void)
{
  rqueue_t rq;

  rq = malloc(sizeof(*rq));

  rq->q = queue_new();
  pthread_mutex_init(&rq->m, NULL);

  return rq;
}

bool
rqueue_is_empty(rqueue_t rq)
{
  bool b;

  pthread_mutex_lock(&rq->m);
  b = queue_is_empty(rq->q);
  pthread_mutex_unlock(&rq->m);

  return b;
}

void
rqueue_add(rqueue_t rq, void *v)
{
  pthread_mutex_lock(&rq->m);
  queue_add(rq->q, v);
  pthread_mutex_unlock(&rq->m);
}

void *
rqueue_remove(rqueue_t rq)
{
  void *v;

  pthread_mutex_lock(&rq->m);
  v = queue_remove(rq->q);
  pthread_mutex_unlock(&rq->m);

  return v;
}

void
rqueue_free(rqueue_t rq)
{
  queue_free(rq->q);
  pthread_mutex_destroy(&rq->m);
  free(rq);
}
