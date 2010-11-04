#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

typedef struct stack * stack_t;

stack_t stack_new      (void);
bool    stack_is_empty (stack_t);
void    stack_push     (stack_t, void *);
void *  stack_pop      (stack_t);
void    stack_free     (stack_t);

#endif
