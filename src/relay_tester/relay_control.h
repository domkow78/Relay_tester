#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H


#include <Arduino.h>
#include "config.h"

extern const uint8_t relay_on[CHANNEL_COUNT];
extern const uint8_t relay_dir[CHANNEL_COUNT];
extern bool relayState[CHANNEL_COUNT];

// KRYTYCZNA FUNKCJA BEZPIECZEŃSTWA!
// Musi być wywołana jako PIERWSZA instrukcja w setup()
// Zapobiega niekontrolowanemu załączeniu przekaźników podczas restartu
void earlyPinSafeInit();

void initRelays();
void setRelayOn(uint8_t ch);
void setRelayOff(uint8_t ch);
void allRelaysOn();
void allRelaysOff();
void setDirection(uint8_t dir);

#endif