#include "runtime_counter.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include "config.h"

static uint32_t lastTick = 0;

void updateRuntime()
{
    if(currentState == STATE_RUNNING ||
       currentState == STATE_STEP_ON ||
       currentState == STATE_STEP_OFF)
    {
        if(millis() - lastTick >= 1000)
        {
            lastTick = millis();
            state.runtime_seconds++;

            if(state.runtime_seconds % RUNTIME_SAVE_INTERVAL == 0)
                saveStateEEPROM();
        }
    }
}