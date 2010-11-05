#ifndef __RQUEUE_H__
#define __RQUEUE_H__

#include <stdbool.h>

/* - - - - - - - - - - - - - - - - - - - - */

typedef struct rqueue * rqueue_t;

/* - - - - - - - - - - - - - - - - - - - - */

rqueue_t rqueue_new      (void);
bool     rqueue_is_empty (rqueue_t);
void     rqueue_add      (rqueue_t, void *);
void *   rqueue_remove   (rqueue_t);
void     rqueue_free     (rqueue_t);

#endif
