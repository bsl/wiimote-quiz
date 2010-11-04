#ifndef __ENDING_H__
#define __ENDING_H__

#include <stdbool.h>

typedef struct ending_s * ending_t;

ending_t ending_new  (bool);
bool     ending_get  (ending_t);
bool     ending_set  (ending_t, bool);
void     ending_free (ending_t);

#endif
