// Copyright Konstantin Bakanov 2017

#include "actions.h"

#define CHECK_RC(x) if (x != FSM_SUCCESS) { printf ("function %s failed\n", #x); }

int main (int argc, char** argv)
{
  // 1. create the fsm
  wfsm fsm = wfsm_create ();
  wfsm_set_nop_event (fsm, NOP);
  wfsm_set_default_state (fsm, "LISTEN");
  // 2. create the states (just an enum)
  // 3. create the events (the return values)
  //    (events trigger actions)
  // 4. connect the states with events and corresponding actions
  
  action* read_check_ac   = read_check_ac_create ();
  action* process_ac      = process_ac_create ();
  action* reply_hshake_ac = reply_hshake_ac_create ();
  action* reply_err_ac    = reply_err_ac_create ();

  CHECK_RC (wfsm_add_transition_str (fsm, "LISTEN", "READ",         ON_IO, read_check_ac));
  CHECK_RC (wfsm_add_transition_str (fsm, "READ",   "LISTEN",       FAILED, NULL));
  CHECK_RC (wfsm_add_transition_str (fsm, "READ", "PROCESS",        SUCCESS, process_ac));
  CHECK_RC (wfsm_add_transition_str (fsm, "PROCESS", "REPLY_HSHAKE",SUCCESS, reply_hshake_ac));
  CHECK_RC (wfsm_add_transition_str (fsm, "PROCESS", "REPLY_ERR",   FAILED, reply_err_ac));
  CHECK_RC (wfsm_add_transition_str (fsm, "REPLY_HSHAKE", "LISTEN", DEFAULT, NULL));
  CHECK_RC (wfsm_add_transition_str (fsm, "REPLY_ERR", "LISTEN",    DEFAULT, NULL));
  
  // now the usage part
  wfsm_process_event (fsm, ON_IO);

  return 0;
}
