#ifndef __MIN_HEAP_H__
#define __MIN_HEAP_H__

#include <stdbool.h>

/* - - - - - - - - - - - - - - - - - - - - */

typedef struct min_heap * min_heap_t;

/* - - - - - - - - - - - - - - - - - - - - */

min_heap_t     min_heap_new          (void);
bool           min_heap_is_empty     (min_heap_t);
int            min_heap_size         (min_heap_t);
void           min_heap_add          (min_heap_t, unsigned long, void *);
void *         min_heap_peek         (min_heap_t);
unsigned long  min_heap_peek_key     (min_heap_t);
void *         min_heap_remove       (min_heap_t);
void           min_heap_decrease_key (min_heap_t, int, unsigned long);
void           min_heap_free         (min_heap_t);

#endif
