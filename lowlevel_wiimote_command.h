#ifndef __LOWLEVEL_WIIMOTE_COMMAND_H__
#define __LOWLEVEL_WIIMOTE_COMMAND_H__

enum lowlevel_wiimote_command_type {
  LOWLEVEL_WIIMOTE_COMMAND_SET_LEDS,
  LOWLEVEL_WIIMOTE_COMMAND_BUZZ
};

struct lowlevel_wiimote_command {
  enum lowlevel_wiimote_command_type type;
  int wiimote_num;
  unsigned long when_to_send;  /* ms since timer_init */

  union {
    struct { int leds;    } set_leds;
    struct { bool active; } buzz;
  } parameters;
};

#endif
