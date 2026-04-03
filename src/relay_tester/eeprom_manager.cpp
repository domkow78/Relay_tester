#include "eeprom_manager.h"
#include "config.h"
#include <EEPROM.h>

TestConfig config;
TestState  state;

// ── Mapa EEPROM ──────────────────────────────────────────────────────────────
//  [0 … 31]   – 1 slot konfiguracji (TestConfig, rzadko zapisywany)
//  [32 … 4095] – ring buffer stanów (271 slotów × 15 bajtów)
//
//  ATmega2560: 4096 bajtów EEPROM, 100 000 zapisów/komórkę
//  271 slotów × 100 000 = 27 100 000 zapisów przed zużyciem

#define EEPROM_TOTAL        4096u
#define EEPROM_CONFIG_ADDR  0u
#define EEPROM_STATE_START  32u
#define STATE_SLOT_SIZE     ((uint16_t)sizeof(TestState))
#define STATE_SLOT_COUNT    ((uint16_t)((EEPROM_TOTAL - EEPROM_STATE_START) / STATE_SLOT_SIZE))

static uint16_t currentSlot = 0;

// ── CRC-16 (Modbus) ──────────────────────────────────────────────────────────
static uint16_t crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++)
            crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : crc >> 1;
    }

    return crc;
}

// ── Config ───────────────────────────────────────────────────────────────────
void loadConfigEEPROM()
{
    EEPROM.get(EEPROM_CONFIG_ADDR, config);

    uint16_t c = crc16((uint8_t*)&config, sizeof(TestConfig) - 2);

    if(c != config.crc || config.step_delay == 0)
    {
        config.step_delay       = DEFAULT_STEP_DELAY;
        config.target_cycles    = DEFAULT_TARGET_CYCLES;
        config.measure_interval = DEFAULT_MEASURE_INTERVAL;
        config.save_interval    = DEFAULT_SAVE_INTERVAL;

        saveConfigEEPROM();
    }
}

void saveConfigEEPROM()
{
    config.crc = crc16((uint8_t*)&config, sizeof(TestConfig) - 2);
    EEPROM.put(EEPROM_CONFIG_ADDR, config);
}

void resetConfigEEPROM()
{
    config.step_delay       = DEFAULT_STEP_DELAY;
    config.target_cycles    = DEFAULT_TARGET_CYCLES;
    config.measure_interval = DEFAULT_MEASURE_INTERVAL;
    config.save_interval    = DEFAULT_SAVE_INTERVAL;
    saveConfigEEPROM();
}

// ── State – ring buffer wear leveling ────────────────────────────────────────
//
//  Zapis: każdy kolejny saveStateEEPROM() trafia do następnego slotu w pierścieniu.
//  Odczyt: skanowanie wszystkich slotów, wybór tego z najwyższym cycle_counter
//          i poprawnym CRC. Następny zapis idzie do slotu po znalezionym.

void loadStateEEPROM()
{
    TestState best;
    bool      found    = false;
    uint16_t  bestSlot = 0;

    for(uint16_t i = 0; i < STATE_SLOT_COUNT; i++)
    {
        TestState s;
        EEPROM.get(EEPROM_STATE_START + i * STATE_SLOT_SIZE, s);

        if(crc16((uint8_t*)&s, sizeof(TestState) - 2) == s.crc)
        {
            // Wybierz slot z najwyższym numerem sekwencji (najnowszy zapis)
            if(!found || s.sequence > best.sequence)
            {
                best     = s;
                bestSlot = i;
                found    = true;
            }
        }
    }

    if(found)
    {
        state       = best;
        currentSlot = (bestSlot + 1) % STATE_SLOT_COUNT;  // następny wolny slot
    }
    else
    {
        // żaden slot niepoprawny – wartości domyślne
        state.sequence           = 0;
        state.cycle_counter      = 0;
        state.runtime_seconds    = 0;
        state.power_fail_counter = 0;
        state.direction          = DIR_LEFT;
        currentSlot              = 0;
    }
}

void saveStateEEPROM()
{
    state.sequence++;  // inkrementuj numer sekwencji przy każdym zapisie
    state.crc = crc16((uint8_t*)&state, sizeof(TestState) - 2);
    EEPROM.put(EEPROM_STATE_START + currentSlot * STATE_SLOT_SIZE, state);
    currentSlot = (currentSlot + 1) % STATE_SLOT_COUNT;
}