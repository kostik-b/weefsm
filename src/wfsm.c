// Copyright Konstantin Bakanov 2017

#include "wfsm.h"

// the below macro resizes any array really and copies the
// contents of the old array over if it does exist
// it does NOT update the size of the array
#define RESIZE_PTR_ARRAY(ELEM_TYPE, arr_ptr, prev_size, new_size)\
  ELEM_TYPE* temp_arr = calloc (new_size, sizeof (ELEM_TYPE));   \
  if ((prev_size > 0) && (arr_ptr))                              \
  { \
    memcpy (temp_arr, arr_ptr, prev_size * sizeof (ELEM_TYPE));  \
    free (arr_ptr);                                              \
  } \
  arr_ptr = temp_arr;

typedef struct transition
{
  int     m_end_state;
  action* m_action;
} transition;

typedef struct wfsm_impl
{
  transition***       m_transition_table; // first index is the starting state
                                    // second one is the event
                                    // the last * is referring to a transition
  size_t              m_table_size; // num of states (first index)
  size_t*             m_events_sizes; // num of events per state (second index)
  unsigned            m_current_state;
  unsigned            m_nop_event;
  // instead of creating an enum of states, we also allow
  // to specify states with strings and we generate the 
  // int values ourselves
  const char**        m_state_names;
} wfsm_impl;

wfsm_impl* wfsm_create ()
{
  wfsm_impl* self = malloc (sizeof (wfsm_impl));
  if (!self)
  {
    return NULL;
  }

  self->m_transition_table = NULL;
  self->m_table_size       = 0;
  self->m_events_size      = NULL;
  self->m_current_state    = 0;
  self->m_nop_event        = 0;

  return self;
}

wfsm_rc  wfsm_add_transition  (
  wfsm_impl* self,
  const unsigned  start_state,
  const unsigned  end_state,
  const unsigned  event,
  action*         ac)
{
  // 1. check if table size is big enough
  //    if not, create a new transition table
  //    copy all the old one over and free the old one
  //    update the table size
  // 2. check if events size is big enough
  //    if not, create a new one and copy the old one over
  //    then free the old one
  // 3. finally add the transition unless it already exists
  //    in which case replace it over

  // 1. indexing the starting state
  if (self->m_table_size < start_state + 1)
  {
    const size_t new_table_size = start_state + 1;

    // allocate the new table (starting states)
    RESIZE_PTR_ARRAY (transition**, self->m_transition_table,
                                    self->m_table_size,
                                    new_table_size);

    // we also have to update the events size array
    RESIZE_PTR_ARRAY(size_t, self->m_events_sizes,
                             self->m_table_size,
                             new_table_size);
  
    self->m_table_size = new_table_size;
  }
  // 2. indexing the event
  if (self->m_events_sizes[start_state] < 1)
  {
    // need to resize also
    RESIZE_PTR_ARRAY (transition*,
                      self->m_transition_table[start_state],
                      (self->m_events_sizes[start_state],
                      event + 1);
    self->m_events_sizes[start_state] = event + 1;
  }
  // 3. ... and finally here is the transition itself
  if (self->m_transition_table[start_state][event] == NULL)
  {
    // create a transition
    transition* tr = malloc (sizeof (transition));
    tr->m_end_state = end_state;
    tr->m_action    = ac;

    self->m_transition_table[start_state][event] = tr;
    return FSM_SUCCESS;
  }
  else
  {
    return FSM_ERROR_TRANSITION_EXISTS;
  }
}
// this is an "internal" function
wfsm_rc wfsm_set_state(
  wfsm            self,
  const unsigned  default_state)
{
  if (default_state > self->m_table_size - 1)
  {
    return FSM_ERROR_START_STATE_DOES_NOT_EXIST;
  }
  else
  {
    self->m_current_state = default_state;
    return FSM_SUCCESS;
  }
}
// this is an "external" function - just a different name really
wfsm_rc wfsm_set_default_state(
  wfsm            self,
  const unsigned  default_state)
{
  return wfsm_set_state (self, default_state);
}

wfsm_rc wfsm_process_event (wfsm self, const unsigned event)
{
  // 1. current state is the starting state
  // 2. index the event
  unsigned current_event = event;
  do
  {
    if (current_event > self->m_event_sizes[self->m_current_state] - 1)
    {
      return FSM_ERROR_EVENT;
    }
    if (self->m_transitions_table[self->m_current_state][current_event] == NULL)
    {
      return FSM_ERROR_TRANSITION_DOES_NOT_EXIST;
    }
    // 3. invoke action
    transition* tr = self->m_transitions_table[self->m_current_state][current_event];
    current_event = *(tr->action->process_event)(tr->action, current_event); 
    // 4. move to the next state
    wfsm_rc rc = wfsm_set_state (self, tr->m_end_state);
    if (rc != FSM_SUCCESS)
    {
      return rc;
    }
  } while (current_event != self->m_nop_event);

  return FSM_SUCCESS;
}

void wfsm_set_nop_event (wfsm self, const unsigned event)
{
  self->m_nop_event (event);
}

static unsigned add_new_state (
  wfsm        self,
  const char* state_string)
{
  const unsigned state_index = self->m_string_states_size;
  RESIZE_PTR_ARR (const char*, self->m_string_states,
                  self->m_string_states_size,
                  self->m_string_states_size + 1);

  self->m_string_states[state_index] = state_string;
  self->m_string_states_size += 1;

  return state_index;
}

wfsm_rc wfsm_add_transition_str(
  wfsm            self,
  const char*     start_state,
  const char*     end_state,
  const unsigned  event,
  action*         ac)
{
  // find the indices for start state and end state
  int start_state_idx = -1;
  int end_state_idx   = -1;

  for (int i = 0; i < self->m_string_states_size; ++i)
  {
    if (strcmp (start_state, self->m_string_states[i]) == 0)
    {
      start_state_idx = i;
    }
    if (strcmp (end_state, self->m_string_states[i]) == 0)
    {
      end_state_idx = i;
    }
    if ((start_state_idx != -1) && (end_state_idx != -1))
    {
      break;
    }
  }

  if (start_state_idx == -1)
  {
    start_state_idx = add_new_state (self, start_state);
  }
  if (end_state_idx == -1)
  {
    end_state_idx = add_new_state (self, end_state);
  }
  if ((start_state_idx < 0) || (end_state_idx < 0))
  {
    return FSM_FAILURE;
  }

  return wfsm_add_transition (self, start_state_idx, end_state_idx, event, ac);
}
