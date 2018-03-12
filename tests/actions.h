// Copyright Konstantin Bakanov 2018

#ifndef kb_actions_h
#define kb_actions_h

#include <wfsm.h>
#include <stdio.h>

/*
  these events are two-fold
  1) the are the return values
     of all the action functions
  2) they are the external events
     that can be passed into the
     fsm
  I.e. each action designed should return one of the
  values from the scheduler_events enum.
*/

typedef enum test_events
{
  NOP,
  DEFAULT,
  ON_IO,
  FAILED,
  SUCCESS
} test_events;

const unsigned dummy_function (action* self, const unsigned event)
{
  printf ("Executing action %s\n", (char*)self->m_impl);

  return SUCCESS;
}

const unsigned default_function (action* self, const unsigned event)
{
  printf ("Executing action %s\n", (char*)self->m_impl);

  return DEFAULT;
}

action* read_check_ac_create ()
{
  action* ac = calloc (1, sizeof (action));

  ac->m_impl = "READ and CHECK";
  ac->process_event = dummy_function;

  return ac;
}

action* process_ac_create ()
{
  action* ac = calloc (1, sizeof (action));

  ac->m_impl = "PROCESS";
  ac->process_event = dummy_function;

  return ac;
}

action* reply_hshake_ac_create ()
{
  action* ac = calloc (1, sizeof (action));

  ac->m_impl = "REPLY HSHAKE";
  ac->process_event = default_function;

  return ac;
}

action* reply_err_ac_create ()
{
  action* ac = calloc (1, sizeof (action));

  ac->m_impl = "REPLY ERR";
  ac->process_event = default_function;

  return ac;
}




#endif
