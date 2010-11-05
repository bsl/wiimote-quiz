#ifndef __LOWLEVEL_COMMAND_H__
#define __LOWLEVEL_COMMAND_H__

#include <stdbool.h>

/* - - - - - - - - - - - - - - - - - - - - */

enum lowlevel_command_type {
  LOWLEVEL_COMMAND_SET_LEDS,
  LOWLEVEL_COMMAND_RUMBLE,
  LOWLEVEL_COMMAND_UNKNOWN
};

struct lowlevel_command {
  int wiimote_num;
  enum lowlevel_command_type type;

  union {
    struct { int leds;    } set_leds;
    struct { bool active; } rumble;
  } parameters;
};

/* - - - - - - - - - - - - - - - - - - - - */

struct lowlevel_command * lowlevel_command_new  (int wiimote_num, enum lowlevel_command_type type);
void                      lowlevel_command_free (struct lowlevel_command *);

#endif
