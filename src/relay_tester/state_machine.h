#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

enum SystemState
{
    STATE_INIT,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_STEP_ON,
    STATE_STEP_OFF,
    STATE_CHANGE_DIRECTION,
    STATE_WAIT_MEASUREMENT,
    STATE_TEST_MODE,
    STATE_FINISHED
};

extern SystemState currentState;

void initStateMachine();
void updateStateMachine();

#endif