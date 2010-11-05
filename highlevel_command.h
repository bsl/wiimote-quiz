#ifndef __HIGHLEVEL_COMMAND_H__
#define __HIGHLEVEL_COMMAND_H__

enum highlevel_command_type {
  HIGHLEVEL_COMMAND_FLASH,
  HIGHLEVEL_COMMAND_RUMBLE,
  HIGHLEVEL_COMMAND_UNKNOWN
};

struct highlevel_command {
  int wiimote_num;
  enum highlevel_command_type type;

  union {
    struct { int pattern_num; } rumble;
    struct { int pattern_num; } flash;
  } parameters;
};

/* - - - - - - - - - - - - - - - - - - - - */

struct highlevel_command * highlevel_command_new  (void);
void                       highlevel_command_free (struct highlevel_command *);

#endif
