#include "relay_control.h"
#include <avr/io.h>

const uint8_t relay_on[CHANNEL_COUNT] =
{19,17,15,23,3,5,7,9,11,13};
// zamiana 1 -> 23

const uint8_t relay_dir[CHANNEL_COUNT] =
{18,16,14,22,2,4,6,8,10,12};
// zamiana 0 --> 22

bool relayState[CHANNEL_COUNT] = {0};

void earlyPinSafeInit()
{
    // ================================================================
    // KRYTYCZNA FUNKCJA BEZPIECZEŃSTWA!
    // Musi być wywołana jako PIERWSZA instrukcja w setup()
    // ================================================================
    //
    // Ustawia wszystkie piny relay_on i relay_dir jako OUTPUT LOW
    // używając bezpośredniego dostępu do rejestrów AVR.
    // Zapobiega niekontrolowanemu załączeniu przekaźników podczas restartu.
    //
    // Bezpośredni dostęp do rejestrów jest SZYBSZY niż pinMode/digitalWrite
    // i wykonuje się w kilka mikrosekund.
    
    // ========== PINY RELAY_ON: 19,17,15,23,3,5,7,9,11,13 ==========
    // Pin 3 = PE5, Pin 5 = PE3, Pin 7 = PH4, Pin 9 = PH6
    // Pin 11 = PB5, Pin 13 = PB7, Pin 15 = PJ0, Pin 17 = PH0
    // Pin 19 = PD2, Pin 23 = PA1
    
    // ========== PINY RELAY_DIR: 18,16,14,22,2,4,6,8,10,12 ==========
    // Pin 2 = PE4, Pin 4 = PG5, Pin 6 = PH3, Pin 8 = PH5
    // Pin 10 = PB4, Pin 12 = PB6, Pin 14 = PJ1, Pin 16 = PH1
    // Pin 18 = PD3, Pin 22 = PA0
    
    // --- Port A: pin 23 (PA1) relay_on, pin 22 (PA0) relay_dir ---
    PORTA &= ~((1 << PA1) | (1 << PA0));  // LOW najpierw!
    DDRA |= (1 << PA1) | (1 << PA0);       // OUTPUT
    
    // --- Port B: piny 11 (PB5), 13 (PB7) relay_on, piny 10 (PB4), 12 (PB6) relay_dir ---
    PORTB &= ~((1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6));
    DDRB |= (1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6);
    
    // --- Port D: pin 19 (PD2) relay_on, pin 18 (PD3) relay_dir ---
    PORTD &= ~((1 << PD2) | (1 << PD3));
    DDRD |= (1 << PD2) | (1 << PD3);
    
    // --- Port E: piny 3 (PE5), 5 (PE3) relay_on, pin 2 (PE4) relay_dir ---
    PORTE &= ~((1 << PE5) | (1 << PE3) | (1 << PE4));
    DDRE |= (1 << PE5) | (1 << PE3) | (1 << PE4);
    
    // --- Port G: pin 4 (PG5) relay_dir ---
    PORTG &= ~(1 << PG5);
    DDRG |= (1 << PG5);
    
    // --- Port H: piny 7 (PH4), 9 (PH6), 17 (PH0) relay_on ---
    // ---         piny 6 (PH3), 8 (PH5), 16 (PH1) relay_dir ---
    PORTH &= ~((1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1));
    DDRH |= (1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1);
    
    // --- Port J: pin 15 (PJ0) relay_on, pin 14 (PJ1) relay_dir ---
    PORTJ &= ~((1 << PJ0) | (1 << PJ1));
    DDRJ |= (1 << PJ0) | (1 << PJ1);
}

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
    if(ch >= CHANNEL_COUNT) return;
    digitalWrite(relay_on[ch], HIGH);
    relayState[ch] = true;
}

void setRelayOff(uint8_t ch)
{
    if(ch >= CHANNEL_COUNT) return;
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