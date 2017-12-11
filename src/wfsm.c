// Copyright QUB 2017

#include "wfsm.h"

typedef struct trasition
{
  int     m_end_state;
  action* m_action;
} transition;

wfsm* wfsm_create ()
{
  wfsm* self = malloc (sizeof (wfsm));
  if (!self)
  {
    return NULL;
  }

  self->m_transitions = NULL;

  return self;
}

wfsm_rc  wfsm_add_transition  (
  wfsm*      self,
  const int start_state,
  const int end_state,
  const int event,
  action*   ac)
{
}
