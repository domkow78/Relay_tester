#include "relay_control.h"
#include <avr/io.h>

const uint8_t relay_on[CHANNEL_COUNT] =
{19,17,15,23,3,5,7,9,11,13};
// zamiana 1 -> 23

const uint8_t relay_dir[CHANNEL_COUNT] =
{18,16,14,22,2,4,6,8,10,12};
// zamiana 0 --> 22

bool relayState[CHANNEL_COUNT] = {0};

// Wczesna inicjalizacja - wywoływana PRZED main()/setup()
// Zapobiega niekontrolowanemu załączeniu przekaźników podczas restartu
void __attribute__((constructor(101))) earlyRelayInit()
{
    // Natychmiast ustaw wszystkie piny relay_on i relay_dir jako OUTPUT LOW
    // używając bezpośredniego dostępu do rejestrów dla maksymalnej szybkości
    
    // ========== PINY RELAY_ON: 19,17,15,23,3,5,7,9,11,13 ==========
    // Na Arduino Mega:
    // Pin 3 = PE5, Pin 5 = PE3, Pin 7 = PH4, Pin 9 = PH6
    // Pin 11 = PB5, Pin 13 = PB7, Pin 15 = PJ0, Pin 17 = PH0
    // Pin 19 = PD2, Pin 23 = PA1
    
    // ========== PINY RELAY_DIR: 18,16,14,22,2,4,6,8,10,12 ==========
    // Na Arduino Mega:
    // Pin 2 = PE4, Pin 4 = PG5, Pin 6 = PH3, Pin 8 = PH5
    // Pin 10 = PB4, Pin 12 = PB6, Pin 14 = PJ1, Pin 16 = PH1
    // Pin 18 = PD3, Pin 22 = PA0
    
    // Port A - pin 23 (PA1) relay_on, pin 22 (PA0) relay_dir
    DDRA |= (1 << PA1) | (1 << PA0);
    PORTA &= ~((1 << PA1) | (1 << PA0));
    
    // Port B - piny 11 (PB5), 13 (PB7) relay_on, piny 10 (PB4), 12 (PB6) relay_dir
    DDRB |= (1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6);
    PORTB &= ~((1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6));
    
    // Port D - pin 19 (PD2) relay_on, pin 18 (PD3) relay_dir
    DDRD |= (1 << PD2) | (1 << PD3);
    PORTD &= ~((1 << PD2) | (1 << PD3));
    
    // Port E - piny 3 (PE5), 5 (PE3) relay_on, pin 2 (PE4) relay_dir
    DDRE |= (1 << PE5) | (1 << PE3) | (1 << PE4);
    PORTE &= ~((1 << PE5) | (1 << PE3) | (1 << PE4));
    
    // Port G - pin 4 (PG5) relay_dir
    DDRG |= (1 << PG5);
    PORTG &= ~(1 << PG5);
    
    // Port H - piny 7 (PH4), 9 (PH6), 17 (PH0) relay_on, piny 6 (PH3), 8 (PH5), 16 (PH1) relay_dir
    DDRH |= (1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1);
    PORTH &= ~((1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1));
    
    // Port J - pin 15 (PJ0) relay_on, pin 14 (PJ1) relay_dir
    DDRJ |= (1 << PJ0) | (1 << PJ1);
    PORTJ &= ~((1 << PJ0) | (1 << PJ1));
}

void initRelays()
{
    // Piny relay_on już zostały zainicjalizowane w earlyRelayInit()
    // Tutaj inicjalizujemy piny relay_dir i upewniamy się, że wszystko jest OK
    
    // Najpierw ustaw relay_dir jako OUTPUT LOW
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        digitalWrite(relay_dir[i],LOW);
        pinMode(relay_dir[i],OUTPUT);
        digitalWrite(relay_dir[i],LOW);
    }

    // Ponownie upewnij się że relay_on są LOW (już powinny być z earlyRelayInit)
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        pinMode(relay_on[i],OUTPUT);  // ponowne ustawienie dla pewności
        digitalWrite(relay_on[i],LOW);
    }
    
    // Wyzeruj stany
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        relayState[i] = false;
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