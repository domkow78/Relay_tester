#include "uart_cli.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include "relay_control.h"
#include "config.h"

static String cmd;

static void sendStatus()
{
    Serial.print("STATE=");
    Serial.print(currentState);

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

static void processCommand(String c)
{
    c.trim();

    if(c == "PING")
        Serial.println("OK");

    else if(c == "STATUS")
        sendStatus();

    else if(c == "CONFIG")
        sendConfig();

    else if(c == "START")
        currentState = STATE_RUNNING;

    else if(c == "PAUSE")
        currentState = STATE_IDLE;

    else if(c == "CONTINUE")
        currentState = STATE_RUNNING;

    else if(c == "TEST")
    {
        allRelaysOn();
        currentState = STATE_TEST_MODE;
    }

    else if(c == "RELEASE")
    {
        allRelaysOff();
        currentState = STATE_IDLE;
    }

    else if(c.startsWith("SET_DELAY"))
    {
        config.step_delay = c.substring(10).toInt();
        saveConfigEEPROM();
    }

    else if(c.startsWith("SET_TARGET"))
    {
        config.target_cycles = c.substring(11).toInt();
        saveConfigEEPROM();
    }

    else if(c.startsWith("SET_INTERVAL"))
    {
        config.measure_interval = c.substring(13).toInt();
        saveConfigEEPROM();
    }

    else if(c == "HELP")
    {
        Serial.println("PING STATUS CONFIG START PAUSE CONTINUE TEST RELEASE SET_DELAY SET_TARGET SET_INTERVAL HELP");
    }
}

void processUART()
{
    while(Serial.available())
    {
        char c = Serial.read();

        if(c == '\n')
        {
            processCommand(cmd);
            cmd = "";
        }
        else
            cmd += c;
    }
}