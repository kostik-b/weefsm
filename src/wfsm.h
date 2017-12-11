// Copyright QUB 2017

#ifndef wfsm_randrom_text_h
#define wfsm_randrom_text_h

typedef enum wfsm_rc
{
  FSM_SUCCESS,
  FSM_FAILURE
} wfsm_rc;

typedef struct action
{
  const int (*process_event) (void* self, const int event);

  void* m_impl;
} action;

typedef struct wfsm
{
  transition** m_transitions; // first index is the starting state
                              // second one is the event
} wfsm;

wfsm*    wfsm_create          ();
wfsm_rc  wfsm_add_transition  (wfsm* self, const int start_state,
                                        const int end_state,
                                        const int event,
                                        action*   ac);
int     wfsm_process_event   (wfsm* self, const int event);

#endif
