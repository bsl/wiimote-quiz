#ifndef __HIGHLEVEL_WIIMOTE_COMMAND_H__
#define __HIGHLEVEL_WIIMOTE_COMMAND_H__

enum highlevel_wiimote_command_type {
  HIGHLEVEL_WIIMOTE_COMMAND_FLASH,
  HIGHLEVEL_WIIMOTE_COMMAND_BUZZ
};

struct highlevel_wiimote_command {
  int wiimote_num;
  enum highlevel_wiimote_command_type type;

  union {
    struct { int pattern_num; } buzz;
    struct { int pattern_num; } flash;
  } parameters;
};

#endif
