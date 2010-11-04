#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

typedef struct queue * queue_t;

queue_t queue_new      (void);
bool    queue_is_empty (queue_t);
void    queue_add      (queue_t, void *);
void *  queue_remove   (queue_t);
void    queue_free     (queue_t);

#endif
