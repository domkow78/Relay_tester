#include "uart_cli.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include "relay_control.h"
#include "lcd_display.h"
#include "config.h"
#include <string.h>

#define CMD_BUFFER_SIZE 32
static char cmd[CMD_BUFFER_SIZE];
static uint8_t cmdIndex = 0;

static const char* stateToString(SystemState s)
{
    switch(s)
    {
        case STATE_INIT:             return "INIT";
        case STATE_IDLE:             return "IDLE";
        case STATE_RUNNING:          return "RUNNING";
        case STATE_STEP_ON:          return "STEP_ON";
        case STATE_STEP_OFF:         return "STEP_OFF";
        case STATE_WAIT_DEAD_TIME:   return "WAIT_DEAD_TIME";
        case STATE_CHANGE_DIRECTION: return "CHANGE_DIRECTION";
        case STATE_WAIT_MEASUREMENT: return "WAIT_MEASUREMENT";
        case STATE_TEST_MODE:        return "TEST_MODE";
        case STATE_FINISHED:         return "FINISHED";
        case STATE_ERROR:            return "ERROR";
        default:                     return "UNKNOWN";
    }
}

static void sendStatus()
{
    Serial.print("STATE=");
    Serial.print(stateToString(currentState));

    Serial.print(";CYCLES=");
    Serial.print(state.cycle_counter);

    Serial.print(";DIR=");
    Serial.print(state.direction == DIR_LEFT ? "LEFT":"RIGHT");

    Serial.print(";TARGET=");
    Serial.print(config.target_cycles);

    Serial.print(";POWER_FAILS=");
    Serial.print(state.power_fail_counter);

    Serial.print(";RUNTIME_H=");
    Serial.print(state.runtime_seconds/3600);

    Serial.print(";FW=");
    Serial.println(FW_VERSION);
}

static void sendConfig()
{
    Serial.println("CONFIGURATION:");

    Serial.print("STEP_DELAY=");
    Serial.println(config.step_delay);

    Serial.print("TARGET_CYCLES=");
    Serial.println(config.target_cycles);

    Serial.print("MEASURE_INTERVAL=");
    Serial.println(config.measure_interval);

    Serial.print("SAVE_INTERVAL=");
    Serial.println(config.save_interval);

    Serial.print("FW=");
    Serial.println(FW_VERSION);
}

// Pomocnicza funkcja - sprawdza czy test jest w trakcie pracy
static bool isTestRunning()
{
    return currentState == STATE_RUNNING ||
           currentState == STATE_STEP_ON ||
           currentState == STATE_STEP_OFF ||
           currentState == STATE_WAIT_DEAD_TIME ||
           currentState == STATE_CHANGE_DIRECTION;
}

static void processCommand(const char* c)
{
    // Pomiń białe znaki na początku
    while(*c == ' ' || *c == '\t' || *c == '\r') c++;

    if(strcmp(c, "PING") == 0)
        Serial.println("OK");

    else if(strcmp(c, "STATUS") == 0)
        sendStatus();

    else if(strcmp(c, "CONFIG") == 0)
        sendConfig();

    else if(strcmp(c, "START") == 0)
    {
        if(currentState == STATE_IDLE || currentState == STATE_WAIT_MEASUREMENT)
            currentState = STATE_RUNNING;
        else
            Serial.println("ERR:INVALID_STATE");
    }

    else if(strcmp(c, "STOP") == 0)
    {
        if(isTestRunning())
        {
            allRelaysOff();
            currentState = STATE_IDLE;
        }
        else
            Serial.println("ERR:INVALID_STATE");
    }

    else if(strcmp(c, "RESET") == 0)
    {
        allRelaysOff();
        clearAllStateSlots();  // WYCZYŚĆ WSZYSTKIE SLOTY przed zapisem nowego stanu
        state.sequence = 0;
        state.cycle_counter = 0;
        state.runtime_seconds = 0;
        state.power_fail_counter = 0;
        state.direction = DIR_LEFT;
        setDirection(DIR_LEFT);
        saveStateEEPROM();
        currentState = STATE_IDLE;
        forceUpdateLCD();  // natychmiast odśwież wyświetlacz
        Serial.println("SYSTEM RESET");
    }

    else if(strcmp(c, "TEST") == 0)
    {
        if(currentState == STATE_IDLE || currentState == STATE_WAIT_MEASUREMENT)
        {
            setDirection(DIR_LEFT);  // relay_dir → LOW (OFF) przed włączeniem
            allRelaysOn();
            currentState = STATE_TEST_MODE;
        }
        else
            Serial.println("ERR:INVALID_STATE");
    }

    else if(strcmp(c, "RELEASE") == 0)
    {
        if(currentState == STATE_TEST_MODE)
        {
            allRelaysOff();
            setDirection(state.direction);  // przywróć kierunek
            currentState = STATE_IDLE;
        }
        else
            Serial.println("ERR:INVALID_STATE");
    }

    else if(strncmp(c, "SET_DELAY ", 10) == 0)
    {
        config.step_delay = atol(c + 10);
        saveConfigEEPROM();
    }

    else if(strncmp(c, "SET_TARGET ", 11) == 0)
    {
        config.target_cycles = atol(c + 11);
        saveConfigEEPROM();
    }

    else if(strncmp(c, "SET_INTERVAL ", 13) == 0)
    {
        config.measure_interval = atol(c + 13);
        saveConfigEEPROM();
    }

    else if(strcmp(c, "FACTORY_RESET") == 0)
    {
        allRelaysOff();
        resetConfigEEPROM();  // reset konfiguracji do wartości z config.h
        clearAllStateSlots(); // WYCZYŚĆ WSZYSTKIE SLOTY przed zapisem nowego stanu
        state.sequence = 0;
        state.cycle_counter = 0;
        state.runtime_seconds = 0;
        state.power_fail_counter = 0;
        state.direction = DIR_LEFT;
        setDirection(DIR_LEFT);
        saveStateEEPROM();
        currentState = STATE_IDLE;
        forceUpdateLCD();  // natychmiast odśwież wyświetlacz
        Serial.println("FACTORY RESET OK");
    }

    else if(strcmp(c, "HELP") == 0)
    {
        Serial.println("PING STATUS CONFIG START STOP RESET FACTORY_RESET TEST RELEASE SET_DELAY SET_TARGET SET_INTERVAL HELP");
    }

    else if(strlen(c) > 0)
    {
        Serial.println("ERR:UNKNOWN_CMD");
    }
}

void processUART()
{
    while(Serial.available())
    {
        char c = Serial.read();

        if(c == '\n')
        {
            cmd[cmdIndex] = '\0';  // null-terminate
            processCommand(cmd);
            cmdIndex = 0;
        }
        else if(c != '\r' && cmdIndex < CMD_BUFFER_SIZE - 1)
        {
            cmd[cmdIndex++] = c;
        }
    }
}