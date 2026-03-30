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
    STATE_WAIT_DEAD_TIME,
    STATE_CHANGE_DIRECTION,
    STATE_WAIT_MEASUREMENT,
    STATE_TEST_MODE,
    STATE_FINISHED,
    STATE_ERROR
};

extern SystemState currentState;

void initStateMachine();
void updateStateMachine();
int8_t getActiveChannel();

#endif