#ifndef __BUTTON_EVENT_H__
#define __BUTTON_EVENT_H__

struct button_event {
  int id;
  int button;
};

struct button_event * button_event_new  (void);
void                  button_event_free (struct button_event *);

#endif
