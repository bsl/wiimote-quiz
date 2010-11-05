#ifndef __LLCMDQUEUE_H__
#define __LLCMDQUEUE_H__

#include <stdbool.h>

#include "lowlevel_command.h"

/* - - - - - - - - - - - - - - - - - - - - */

typedef struct llcmdqueue * llcmdqueue_t;

/* - - - - - - - - - - - - - - - - - - - - */

llcmdqueue_t              llcmdqueue_new          (void);
bool                      llcmdqueue_is_empty     (llcmdqueue_t);
bool                      llcmdqueue_size         (llcmdqueue_t);
void                      llcmdqueue_add          (llcmdqueue_t, struct lowlevel_command *, unsigned long);
struct lowlevel_command * llcmdqueue_peek         (llcmdqueue_t);
unsigned long             llcmdqueue_peek_key     (llcmdqueue_t);
struct lowlevel_command * llcmdqueue_remove       (llcmdqueue_t);
void                      llcmdqueue_decrease_key (llcmdqueue_t, int, unsigned long);
void                      llcmdqueue_free         (llcmdqueue_t);

#endif
