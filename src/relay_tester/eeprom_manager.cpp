#include "eeprom_manager.h"
#include "config.h"
#include <EEPROM.h>

TestConfig config;
TestState state;

#define EEPROM_CONFIG_A 0
#define EEPROM_CONFIG_B 32
#define EEPROM_STATE_A 64
#define EEPROM_STATE_B 128

static bool stateSlot = false;

static uint16_t crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i=0;i<len;i++)
    {
        crc ^= data[i];

        for(uint8_t j=0;j<8;j++)
        {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

void loadConfigEEPROM()
{
    EEPROM.get(EEPROM_CONFIG_A, config);

    uint16_t c = crc16((uint8_t*)&config, sizeof(TestConfig)-2);

    if(c != config.crc || config.step_delay == 0)
    {
    config.step_delay = DEFAULT_STEP_DELAY;
    config.target_cycles = DEFAULT_TARGET_CYCLES;
    config.measure_interval = DEFAULT_MEASURE_INTERVAL;
    config.save_interval = DEFAULT_SAVE_INTERVAL;

    saveConfigEEPROM();
    }
    
}

void saveConfigEEPROM()
{
    config.crc = crc16((uint8_t*)&config,sizeof(TestConfig)-2);

    EEPROM.put(EEPROM_CONFIG_A,config);
}

void loadStateEEPROM()
{
    TestState sa, sb;
    EEPROM.get(EEPROM_STATE_A, sa);
    EEPROM.get(EEPROM_STATE_B, sb);

    bool okA = (crc16((uint8_t*)&sa, sizeof(TestState)-2) == sa.crc);
    bool okB = (crc16((uint8_t*)&sb, sizeof(TestState)-2) == sb.crc);

    if(okA && okB)
    {
        // obydwa poprawne – wybierz z wyższym cycle_counter (nowszy zapis)
        if(sb.cycle_counter > sa.cycle_counter)
        {
            state = sb;
            stateSlot = true;   // ostatni zapis był do B → następny do A
        }
        else
        {
            state = sa;
            stateSlot = false;  // ostatni zapis był do A → następny do B
        }
    }
    else if(okA)
    {
        state = sa;
        stateSlot = false;  // ostatni zapis był do A → następny do B
    }
    else if(okB)
    {
        state = sb;
        stateSlot = true;   // ostatni zapis był do B → następny do A
    }
    else
    {
        // żaden slot niepoprawny – wartości domyślne
        state.cycle_counter = 0;
        state.runtime_seconds = 0;
        state.power_fail_counter = 0;
        state.direction = DIR_LEFT;
        stateSlot = false;
    }
}

void saveStateEEPROM()
{
    state.crc = crc16((uint8_t*)&state,sizeof(TestState)-2);

    if(stateSlot)
        EEPROM.put(EEPROM_STATE_A,state);
    else
        EEPROM.put(EEPROM_STATE_B,state);

    stateSlot = !stateSlot;
}