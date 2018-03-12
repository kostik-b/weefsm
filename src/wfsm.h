// Copyright Konstantin Bakanov 2017

#ifndef kb_wfsm_h
#define kb_wfsm_h

#include <stdlib.h>

typedef enum wfsm_rc
{
  FSM_SUCCESS,
  FSM_FAILURE,
  FSM_ERROR_MEM_ALLOC,
  FSM_ERROR_EVENT,
  FSM_ERROR_TRANSITION_EXISTS,
  FSM_ERROR_START_STATE_DOES_NOT_EXIST,
  FSM_ERROR_TRANSITION_DOES_NOT_EXIST
} wfsm_rc;

typedef struct action
{
  const unsigned (*process_event) (void* self, const unsigned event);

  void* m_impl;
} action;

// forward declaration
struct wfsm_impl;
typedef struct wfsm_impl* wfsm;

wfsm     wfsm_create          ();

wfsm_rc  wfsm_add_transition_str  (wfsm self, const char*     start_state,
                                              const char*     end_state,
                                              const unsigned  event,
                                              action*         ac);

wfsm_rc wfsm_set_default_state    (wfsm self, const char* default_state);
// the idea here is that an event triggers the action (and the
// corresponding state transition if there is any attached)
// AND the action also returns either (1) another event
// or (2) NOP, i.e. we stop there.
wfsm_rc wfsm_process_event        (wfsm self, const unsigned event);

void    wfsm_set_nop_event        (wfsm self, const unsigned event);

#endif
