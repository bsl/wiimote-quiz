#ifndef __HIGHLEVEL_WIIMOTE_COMMAND_H__
#define __HIGHLEVEL_WIIMOTE_COMMAND_H__

enum highlevel_wiimote_command_type {
  HIGHLEVEL_WIIMOTE_COMMAND_FLASH,
  HIGHLEVEL_WIIMOTE_COMMAND_RUMBLE,
  HIGHLEVEL_WIIMOTE_COMMAND_UNKNOWN
};

struct highlevel_wiimote_command {
  int wiimote_num;
  enum highlevel_wiimote_command_type type;

  union {
    struct { int pattern_num; } rumble;
    struct { int pattern_num; } flash;
  } parameters;
};

/* - - - - - - - - - - - - - - - - - - - - */

struct highlevel_wiimote_command * highlevel_wiimote_command_new  (void);
void                               highlevel_wiimote_command_free (struct highlevel_wiimote_command *);

#endif
