#include "relay_control.h"

const uint8_t relay_on[CHANNEL_COUNT] =
{22,23,24,25,26,27,28,29,30,31};

const uint8_t relay_dir[CHANNEL_COUNT] =
{32,33,34,35,36,37,38,39,40,41};

void initRelays()
{
    // najpierw ustaw stan LOW zanim ustawimy pin jako OUTPUT
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        digitalWrite(relay_on[i],LOW);
        digitalWrite(relay_dir[i],LOW);
    }

    // dopiero teraz ustaw OUTPUT
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        pinMode(relay_on[i],OUTPUT);
        pinMode(relay_dir[i],OUTPUT);
    }

    // dodatkowe zabezpieczenie
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        digitalWrite(relay_on[i],LOW);
        digitalWrite(relay_dir[i],LOW);
    }
}

void setRelayOn(uint8_t ch)
{
    digitalWrite(relay_on[ch],HIGH);
}

void setRelayOff(uint8_t ch)
{
    digitalWrite(relay_on[ch],LOW);
}

void allRelaysOn()
{
    for(int i=0;i<CHANNEL_COUNT;i++)
        digitalWrite(relay_on[i],HIGH);
}

void allRelaysOff()
{
    for(int i=0;i<CHANNEL_COUNT;i++)
        digitalWrite(relay_on[i],LOW);
}

void setDirection(uint8_t dir)
{
    for(int i=0;i<CHANNEL_COUNT;i++)
        digitalWrite(relay_dir[i],dir);
}