// Copyright Konstantin Bakanov 2017

#include "wfsm.h"
#include <stdio.h>
#include <string.h>

#define WFSM_DEBUG 1

// the below macro resizes any array really and copies the
// contents of the old array over if it does exist
// it does NOT update the size of the array
#define RESIZE_PTR_ARRAY(ELEM_TYPE, arr_ptr, prev_size, new_size)\
  {                                                                \
    ELEM_TYPE* temp_arr = calloc (new_size, sizeof (ELEM_TYPE));   \
    if ((prev_size > 0) && (arr_ptr))                              \
    { \
      memcpy (temp_arr, arr_ptr, prev_size * sizeof (ELEM_TYPE));  \
      free (arr_ptr);                                              \
    } \
    arr_ptr = temp_arr;                                            \
  }

typedef struct transition
{
  unsigned  m_end_state;
  action*   m_action;
} transition;

typedef struct wfsm_impl
{
  transition***       m_transitions_table; // first index is the starting state
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
  size_t              m_string_states_size;
} wfsm_impl;

wfsm_impl* wfsm_create ()
{
  wfsm_impl* self = calloc (1, sizeof (wfsm_impl));
  if (!self)
  {
    return NULL;
  }

#ifdef WFSM_DEBUG
  printf ("Created the wfsm\n");
#endif

  return self;
}

static wfsm_rc  wfsm_add_transition  (
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

#ifdef WFSM_DEBUG
  printf ("Inserting new transition into transitions table\n");
#endif

  // 1. indexing the starting state
  if (self->m_table_size < start_state + 1)
  {
    const size_t new_table_size = start_state + 1;

#ifdef WFSM_DEBUG
    printf ("  Resizing table to new size %u\n", new_table_size);
#endif

    // allocate the new table (starting states)
    RESIZE_PTR_ARRAY (transition**, self->m_transitions_table,
                                    self->m_table_size,
                                    new_table_size);

    // we also have to update the events size array
    RESIZE_PTR_ARRAY(size_t, self->m_events_sizes,
                             self->m_table_size,
                             new_table_size);
  
    self->m_table_size = new_table_size;
  }
  // 2. indexing the event
  if (self->m_events_sizes[start_state] < event + 1)
  {
#ifdef WFSM_DEBUG
    printf ("  Resizing events array to %u\n", event + 1);
#endif
    // need to resize also
    RESIZE_PTR_ARRAY (transition*,
                      self->m_transitions_table[start_state],
                      self->m_events_sizes[start_state],
                      event + 1);
    self->m_events_sizes[start_state] = event + 1;
  }
  // 3. ... and finally here is the transition itself
  if (self->m_transitions_table[start_state][event] == NULL)
  {
#ifdef WFSM_DEBUG
    printf ("  Creating the actual transition\n");
#endif
    // create a transition
    transition* tr = malloc (sizeof (transition));
    tr->m_end_state = end_state;
    tr->m_action    = ac;

    self->m_transitions_table[start_state][event] = tr;
    return FSM_SUCCESS;
  }
  else
  {
    return FSM_ERROR_TRANSITION_EXISTS;
  }
}

static unsigned add_new_state (
  wfsm        self,
  const char* state_string)
{
#ifdef WFSM_DEBUG
  printf ("Adding new state idx %u for string %s\n", self->m_string_states_size, state_string);
#endif

  const unsigned state_index = self->m_string_states_size;
  RESIZE_PTR_ARRAY (const char*, self->m_state_names,
                  self->m_string_states_size,
                  self->m_string_states_size + 1);

  self->m_state_names[state_index] = strdup(state_string);
  self->m_string_states_size += 1;

  return state_index;
}

// this is an "internal" function
static wfsm_rc wfsm_set_state(
  wfsm            self,
  const unsigned  state)
{
  // at least some sort of check
  if (state > self->m_table_size - 1)
  {
    return FSM_ERROR_START_STATE_DOES_NOT_EXIST;
  }

  self->m_current_state = state;
  return FSM_SUCCESS;
}

// this is an "external" function - just a different name really
wfsm_rc wfsm_set_default_state(
  wfsm        self,
  const char* default_state)
{
  int default_state_idx = -1;  
  for (int i = 0; i < self->m_string_states_size; ++i)
  {
    if (strcmp (default_state, self->m_state_names[i]) == 0)
    {
      default_state_idx = i;
      break;
    }
  }

  if (default_state_idx < 0)
  {
    default_state_idx = add_new_state (self, default_state);
  }

  return wfsm_set_state (self, (unsigned)default_state_idx);
}

static const char* get_state_str (
  wfsm           self,
  const unsigned state_idx)
{
  if (self->m_state_names && (state_idx < self->m_string_states_size)
          && self->m_state_names[state_idx])
  {
    return self->m_state_names[state_idx];
  }
  else
  {
    return "";
  }
}

wfsm_rc wfsm_process_event (wfsm self, const unsigned event)
{
  // 1. current state is the starting state
  // 2. index the event
  unsigned current_event = event;
  do
  {
#ifdef WFSM_DEBUG
    printf ("Processing event %u, current state is %s\n", current_event, get_state_str (self, self->m_current_state));
#endif
    if (current_event > self->m_events_sizes[self->m_current_state] - 1)
    {
      return FSM_ERROR_EVENT;
    }
    if (self->m_transitions_table[self->m_current_state][current_event] == NULL)
    {
#ifdef WFSM_DEBUG
    printf ("There is no transition defined for event %u from state %s\n",
                    current_event, get_state_str (self, self->m_current_state));
#endif
      return FSM_ERROR_TRANSITION_DOES_NOT_EXIST;
    }
    // 3. move to the next state
    transition* tr = self->m_transitions_table[self->m_current_state][current_event];
    wfsm_rc rc = wfsm_set_state (self, tr->m_end_state);
    if (rc != FSM_SUCCESS)
    {
      return rc;
    }
    // 4. invoke action
    if (tr->m_action == NULL)
    {
      current_event = self->m_nop_event;
    }
    else
    {
      current_event = (*(tr->m_action->process_event))(tr->m_action, current_event); 
    }
#ifdef WFSM_DEBUG
  printf ("Finished executing action. The new event is %u. It is %s a NOP event\n", current_event,
                      (current_event == self->m_nop_event) ? "" : "NOT");
  
#endif
  } while (current_event != self->m_nop_event);

  return FSM_SUCCESS;
}

void wfsm_set_nop_event (wfsm self, const unsigned event)
{
  self->m_nop_event = event;
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

#ifdef WFSM_DEBUG
  printf ("Adding transition %s -> %s (%u)\n", start_state, end_state, event);
#endif

  for (int i = 0; i < self->m_string_states_size; ++i)
  {
    if (strcmp (start_state, self->m_state_names[i]) == 0)
    {
      start_state_idx = i;
    }
    if (strcmp (end_state, self->m_state_names[i]) == 0)
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

#ifdef WFSM_DEBUG
  printf ("State %s has idx of %d\n", start_state, start_state_idx);
  printf ("State %s has idx of %d\n", end_state,   end_state_idx);
#endif

  return wfsm_add_transition (self, start_state_idx, end_state_idx, event, ac);
}
