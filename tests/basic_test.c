// Copyright QUB 2017

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
  fsm* state_machine = fsm_create ();
  // 2. create the states (just an enum)
  // 3. create the events (the return values)
  //    (events trigger actions)
  // 4. connect the states with events and corresponding actions

  configure_action* c_ac = configure_action_create ();
  fsm_add_transition (fsm, INITIAL, CONFIGURING, START, c_ac);


  // now the usage part
  fsm_process_event (fsm, START);

  return 0;
}
