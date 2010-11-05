#include <stdio.h>
#include <stdlib.h>

#include "button_event.h"

/* - - - - - - - - - - - - - - - - - - - - */

struct button_event *
button_event_new(int id, int button)
{
  struct button_event *b;

  b = malloc(sizeof(*b));
  b->id = id;
  b->button = button;

  return b;
}

void
button_event_free(struct button_event *b)
{
  free(b);
}
