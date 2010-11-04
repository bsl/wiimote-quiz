#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct stack {
  int size;
  int avail;
  void **ps;
};

/* - - - - - - - - - - - - - - - - - - - - */

static void stack_resize(stack_t s);
static void stack_grow(stack_t s);
static void stack_shrink(stack_t s);

/* - - - - - - - - - - - - - - - - - - - - */

stack_t
stack_new()
{
  stack_t s = malloc(sizeof(*s));
  if (s != NULL) {
    s->size = 0;
    s->avail = 0;
    s->ps = NULL;
  }
  return s;
}

bool
stack_is_empty(stack_t s)
{
  return s->size == 0;
}

void
stack_push(stack_t s, void *p)
{
  if (s->size + 1 > s->avail) {
    stack_grow(s);
  }
  s->ps[s->size] = p;
  s->size++;
}

void *
stack_pop(stack_t s)
{
  s->size--;
  void *p = s->ps[s->size];
  if (s->size < s->avail/2) {
    stack_shrink(s);
  }
  return p;
}

void
stack_free(stack_t s)
{
  if (s->ps) {
    free(s->ps);
  }
  free(s);
}

static void
stack_resize(stack_t s)
{
  s->ps = realloc(s->ps, sizeof(void *)*s->avail);
}

static void
stack_grow(stack_t s)
{
  s->avail = s->avail == 0 ? 1 : s->avail*2;
  stack_resize(s);
}

static void
stack_shrink(stack_t s)
{
  s->avail /= 2;
  stack_resize(s);
}
