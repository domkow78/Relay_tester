#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>

struct TestConfig
{
    uint32_t step_delay;
    uint32_t target_cycles;
    uint32_t measure_interval;
    uint16_t save_interval;
    uint16_t crc;
};

struct TestState
{
    uint32_t sequence;          // numer sekwencji - inkrementowany przy każdym zapisie
    uint32_t cycle_counter;
    uint32_t runtime_seconds;
    uint32_t power_fail_counter;
    uint8_t direction;
    uint16_t crc;
};

extern TestConfig config;
extern TestState state;

void loadConfigEEPROM();
void saveConfigEEPROM();
void resetConfigEEPROM();  // reset konfiguracji do wartości domyślnych z config.h

void loadStateEEPROM();
void saveStateEEPROM();

#endif