#include "config.h"
#include "relay_control.h"
#include "state_machine.h"
#include "uart_cli.h"
#include "lcd_display.h"
#include "eeprom_manager.h"
#include "runtime_counter.h"
#include <avr/wdt.h>

// Deklaracja wczesnej inicjalizacji z relay_control.cpp
extern void earlyRelayInit(void) __attribute__((constructor(101)));

void setup()
{
    // UWAGA: earlyRelayInit() została już wywołana automatycznie przed setup()
    // dzięki __attribute__((constructor)) - piny relay_on są już OUTPUT LOW
    
    wdt_disable();  // wyłącz watchdoga na czas inicjalizacji

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

    wdt_enable(WDTO_2S);  // włącz watchdoga: reset jeśli brak wdt_reset() przez 2 s
}

void loop()
{
    wdt_reset();  // "nakarm" watchdoga
    processUART();
    updateStateMachine();
    updateRuntime();
    updateLCD();
}