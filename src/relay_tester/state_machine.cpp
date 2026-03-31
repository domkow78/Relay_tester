#include "state_machine.h"
#include "relay_control.h"
#include "eeprom_manager.h"
#include "config.h"

SystemState currentState = STATE_INIT;

static uint32_t stepTimer = 0;
static uint8_t stepIndex = 0;
static bool idleRelaysOff = false;
static bool finishedRelaysOff = false;

int8_t getActiveChannel()
{
    if(currentState == STATE_STEP_ON &&
       stepIndex > 0 &&
       stepIndex <= CHANNEL_COUNT)
        return (int8_t)(stepIndex - 1);

    return -1;
}

void initStateMachine()
{
    if(config.measure_interval == 0)
        config.measure_interval = DEFAULT_MEASURE_INTERVAL;

    if(config.target_cycles == 0)
        config.target_cycles = DEFAULT_TARGET_CYCLES;

    if(state.cycle_counter >= config.target_cycles)
    {
        currentState = STATE_FINISHED;
    }
    else if(config.measure_interval != 0 &&
            state.cycle_counter % config.measure_interval == 0 &&
            state.cycle_counter != 0)
    {
        currentState = STATE_WAIT_MEASUREMENT;
    }
    else
    {
        currentState = STATE_RUNNING;
    }
}

void updateStateMachine()
{
    switch(currentState)
    {

        case STATE_IDLE:
            // Stan bezczynności - wyłącz przekaźniki tylko raz
            if(!idleRelaysOff)
            {
                allRelaysOff();
                idleRelaysOff = true;
            }
            break;


        case STATE_FINISHED:
            // Test zakończony - wyłącz przekaźniki tylko raz
            if(!finishedRelaysOff)
            {
                allRelaysOff();
                finishedRelaysOff = true;
            }
            break;


        case STATE_RUNNING:

            stepIndex = 0;
            stepTimer = millis();
            idleRelaysOff = false;
            finishedRelaysOff = false;
            currentState = STATE_STEP_ON;
            break;


        case STATE_STEP_ON:

            if(millis() - stepTimer >= config.step_delay)
            {
                // wyłącz poprzedni kanał (wave – tylko jeden ON na raz)
                if(stepIndex > 0)
                    setRelayOff(stepIndex - 1);

                if(stepIndex < CHANNEL_COUNT)
                {
                    setRelayOn(stepIndex);
                    stepIndex++;
                    stepTimer = millis();
                }
                else
                {
                    // ostatni kanał wyłączony, przejdź do dead time
                    stepTimer = millis();
                    currentState = STATE_WAIT_DEAD_TIME;
                }
            }

            break;


        case STATE_STEP_OFF:
            // nieużywany – sekwencja wave obsługiwana w STATE_STEP_ON
            currentState = STATE_WAIT_DEAD_TIME;
            break;


        case STATE_WAIT_DEAD_TIME:

            if(millis() - stepTimer >= DEAD_TIME)
            {
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


        case STATE_ERROR:
            // Stan błędu krytycznego - wyłącz wszystkie przekaźniki i zatrzymaj system
            allRelaysOff();
            // System pozostaje w tym stanie do ręcznego resetu (komenda RESET lub power cycle)
            break;


        default:
            break;
    }
}