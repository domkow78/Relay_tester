#include "config.h"
#include "relay_control.h"
#include "state_machine.h"
#include "uart_cli.h"
#include "lcd_display.h"
#include "eeprom_manager.h"
#include "runtime_counter.h"

void setup()
{
    Serial.begin(115200);
    
    initRelays();

    allRelaysOff();
    setDirection(DIR_LEFT);

    initLCD();

    loadConfigEEPROM();
    loadStateEEPROM();

    state.power_fail_counter++;
    saveStateEEPROM();

    setDirection(state.direction);

    initStateMachine();
}

void loop()
{
    processUART();
    updateStateMachine();
    updateRuntime();
    updateLCD();
}