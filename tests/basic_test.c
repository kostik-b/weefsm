// Copyright Konstantin Bakanov 2017

typedef enum scheduler_states
{
  INITIAL,
  CONFIGURING
  WORKING
} scheduler_states;

/*
  these events are two-fold
  1) the are the return values
     of all the action functions
  2) they are the external events
     that can be passed into the
     fsm
*/
typedef scheduler_events
{
  NOP,
  FAILED,
  SUCCESS,
  START
} scheduler_events;


int main (int argc, char** argv)
{
  // 1. create the fsm
  wfsm state_machine = wfsm_create ();
  wfsm_set_nop_event (state_machine, NOP);
  // 2. create the states (just an enum)
  // 3. create the events (the return values)
  //    (events trigger actions)
  // 4. connect the states with events and corresponding actions
  
  configure_action* c_ac = configure_action_create ();
  wfsm_add_transition (wfsm, INITIAL, CONFIGURING, START, c_ac);


  // now the usage part
  wfsm_process_event (wfsm, START);

  return 0;
}
