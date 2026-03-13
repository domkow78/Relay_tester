#include "state_machine.h"
#include "relay_control.h"
#include "eeprom_manager.h"
#include "config.h"

SystemState currentState = STATE_INIT;

static uint32_t stepTimer = 0;
static uint8_t stepIndex = 0;

void initStateMachine()
{
    if(state.cycle_counter >= config.target_cycles)
        currentState = STATE_FINISHED;

    else if(state.cycle_counter % config.measure_interval == 0 &&
            state.cycle_counter != 0)
        currentState = STATE_WAIT_MEASUREMENT;

    else
        currentState = STATE_RUNNING;
}

void updateStateMachine()
{
    switch(currentState)
    {

        case STATE_RUNNING:

            stepIndex = 0;
            stepTimer = millis();
            currentState = STATE_STEP_ON;
            break;


        case STATE_STEP_ON:

            if(millis() - stepTimer >= config.step_delay)
            {
                setRelayOn(stepIndex);

                stepIndex++;
                stepTimer = millis();

                if(stepIndex >= CHANNEL_COUNT)
                {
                    stepIndex = 0;
                    currentState = STATE_STEP_OFF;
                }
            }

            break;


        case STATE_STEP_OFF:

            if(millis() - stepTimer >= config.step_delay)
            {
                setRelayOff(stepIndex);

                stepIndex++;
                stepTimer = millis();

                if(stepIndex >= CHANNEL_COUNT)
                    currentState = STATE_CHANGE_DIRECTION;
            }

            break;


        case STATE_CHANGE_DIRECTION:

            state.cycle_counter++;

            if(state.direction == DIR_LEFT)
                state.direction = DIR_RIGHT;
            else
                state.direction = DIR_LEFT;

            setDirection(state.direction);

            if(state.cycle_counter % config.save_interval == 0)
                saveStateEEPROM();

            if(state.cycle_counter >= config.target_cycles)
            {
                currentState = STATE_FINISHED;
                break;
            }

            if(state.cycle_counter % config.measure_interval == 0)
            {
                currentState = STATE_WAIT_MEASUREMENT;
                break;
            }

            currentState = STATE_RUNNING;

            break;

        default:
            break;
    }
}