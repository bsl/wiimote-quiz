#ifndef __LOWLEVEL_WIIMOTE_COMMAND_H__
#define __LOWLEVEL_WIIMOTE_COMMAND_H__

enum lowlevel_wiimote_command_type {
  LOWLEVEL_WIIMOTE_COMMAND_SET_LEDS,
  LOWLEVEL_WIIMOTE_COMMAND_RUMBLE
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

#endif
