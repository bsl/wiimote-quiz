#include <stdbool.h>
#include <stdlib.h>

#include "llcmdqueue.h"
#include "lowlevel_command.h"
#include "min_heap.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct llcmdqueue {
  min_heap_t heap;
};

/* - - - - - - - - - - - - - - - - - - - - */

llcmdqueue_t
llcmdqueue_new(void)
{
  llcmdqueue_t llcq = malloc(sizeof(llcmdqueue_t));
  llcq->heap = min_heap_new();
  return llcq;
}

bool
llcmdqueue_is_empty(llcmdqueue_t q)
{
  return min_heap_is_empty(q->heap);
}

bool
llcmdqueue_size(llcmdqueue_t q)
{
  return min_heap_size(q->heap);
}

void
llcmdqueue_add(llcmdqueue_t q, struct lowlevel_command *llc, unsigned long when)
{
  min_heap_add(q->heap, when, llc);
}

struct lowlevel_command *
llcmdqueue_peek(llcmdqueue_t q)
{
  return min_heap_peek(q->heap);
}

unsigned long
llcmdqueue_peek_key(llcmdqueue_t q)
{
  return min_heap_peek_key(q->heap);
}

struct lowlevel_command *
llcmdqueue_remove(llcmdqueue_t q)
{
  return min_heap_remove(q->heap);
}

void
llcmdqueue_decrease_key(llcmdqueue_t q, int i, unsigned long k)
{
  min_heap_decrease_key(q->heap, i, k);
}

void
llcmdqueue_free(llcmdqueue_t q)
{
  min_heap_free(q->heap);
}
