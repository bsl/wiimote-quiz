#ifndef __LOWLEVEL_WIIMOTE_COMMAND_H__
#define __LOWLEVEL_WIIMOTE_COMMAND_H__

#include <stdbool.h>

/* - - - - - - - - - - - - - - - - - - - - */

enum lowlevel_wiimote_command_type {
  LOWLEVEL_WIIMOTE_COMMAND_SET_LEDS,
  LOWLEVEL_WIIMOTE_COMMAND_RUMBLE,
  LOWLEVEL_WIIMOTE_COMMAND_UNKNOWN
};

struct lowlevel_wiimote_command {
  int wiimote_num;
  enum lowlevel_wiimote_command_type type;
  unsigned long when_to_send;  /* ms since timer_init */

  union {
    struct { int leds;    } set_leds;
    struct { bool active; } rumble;
  } parameters;
};

/* - - - - - - - - - - - - - - - - - - - - */

struct lowlevel_wiimote_command *lowlevel_wiimote_command_new  (void);
void                             lowlevel_wiimote_command_free (struct lowlevel_wiimote_command *);

#endif
