#include <stdlib.h>

#include "queue.h"
#include "stack.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct queue {
  stack_t s1;
  stack_t s2;
};

/* - - - - - - - - - - - - - - - - - - - - */

queue_t
queue_new(void)
{
  queue_t q = malloc(sizeof(*q));
  q->s1 = stack_new();
  q->s2 = stack_new();
  return q;
}

bool
queue_is_empty(queue_t q)
{
  return
    stack_is_empty(q->s1) &&
    stack_is_empty(q->s2);
}

void
queue_add(queue_t q, void *p)
{
  stack_push(q->s1, p);
}

void *
queue_remove(queue_t q)
{
  if (stack_is_empty(q->s2)) {
    while (!stack_is_empty(q->s1)) {
      stack_push(q->s2, stack_pop(q->s1));
    }
  }
  return stack_pop(q->s2);
}

void
queue_free(queue_t q)
{
  stack_free(q->s1);
  stack_free(q->s2);
  free(q);
}
