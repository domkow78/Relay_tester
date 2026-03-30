#include "relay_control.h"

const uint8_t relay_on[CHANNEL_COUNT] =
{19,17,15,23,3,5,7,9,11,13};
// zamiana 1 -> 23

const uint8_t relay_dir[CHANNEL_COUNT] =
{18,16,14,22,2,4,6,8,10,12};
// zamiana 0 --> 22

bool relayState[CHANNEL_COUNT] = {0};

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
    digitalWrite(relay_on[ch], HIGH);
    relayState[ch] = true;
}

void setRelayOff(uint8_t ch)
{
    digitalWrite(relay_on[ch], LOW);
    relayState[ch] = false;
}

void allRelaysOn()
{
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        digitalWrite(relay_on[i],HIGH);
        relayState[i] = true;
    }
}

void allRelaysOff()
{
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        digitalWrite(relay_on[i],LOW);
        relayState[i] = false;
    }
}

void setDirection(uint8_t dir)
{
    for(int i=0;i<CHANNEL_COUNT;i++)
        digitalWrite(relay_dir[i],dir);
}