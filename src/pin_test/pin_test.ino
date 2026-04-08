/*
 * MINIMALNY TEST PINÓW - DIAGNOZA PROBLEMU BOOTLOADERA
 * =====================================================
 * 
 * Ten sketch ma na celu sprawdzenie, czy problem z pinami
 * występuje PRZED uruchomieniem setup() (bootloader)
 * czy PODCZAS inicjalizacji kodu.
 * 
 * INSTRUKCJA TESTU:
 * 1. Wgraj ten sketch na Arduino Mega
 * 2. Obserwuj LED (pin 13) - miga co 500ms gdy kod działa
 * 3. Podłącz oscyloskop/multimetr do testowanych pinów
 * 4. Sprawdź czy piny są stabilne LOW od razu po RESET
 * 
 * PINY RELAY_ON:  19, 17, 15, 23, 3, 5, 7, 9, 11, 13
 * PINY RELAY_DIR: 18, 16, 14, 22, 2, 4, 6, 8, 10, 12
 */

#include <avr/io.h>

// =============================================================
// NATYCHMIASTOWA INICJALIZACJA PRZED main() - SEKCJA .init3
// =============================================================
// Ta funkcja wykonuje się PRZED Arduino setup()!
// Jeśli po tym piny nadal "pulsują" podczas RESET - to bootloader.

void earlyInit() __attribute__((naked, section(".init3")));
void earlyInit()
{
    // Port A: pin 23 (PA1), pin 22 (PA0)
    PORTA &= ~((1 << PA1) | (1 << PA0));
    DDRA |= (1 << PA1) | (1 << PA0);
    
    // Port B: piny 11 (PB5), 13 (PB7), 10 (PB4), 12 (PB6)
    PORTB &= ~((1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6));
    DDRB |= (1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB6);
    
    // Port D: pin 19 (PD2), pin 18 (PD3)
    PORTD &= ~((1 << PD2) | (1 << PD3));
    DDRD |= (1 << PD2) | (1 << PD3);
    
    // Port E: piny 3 (PE5), 5 (PE3), 2 (PE4)
    PORTE &= ~((1 << PE5) | (1 << PE3) | (1 << PE4));
    DDRE |= (1 << PE5) | (1 << PE3) | (1 << PE4);
    
    // Port G: pin 4 (PG5)
    PORTG &= ~(1 << PG5);
    DDRG |= (1 << PG5);
    
    // Port H: piny 7 (PH4), 9 (PH6), 17 (PH0), 6 (PH3), 8 (PH5), 16 (PH1)
    PORTH &= ~((1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1));
    DDRH |= (1 << PH4) | (1 << PH6) | (1 << PH0) | (1 << PH3) | (1 << PH5) | (1 << PH1);
    
    // Port J: pin 15 (PJ0), pin 14 (PJ1)
    PORTJ &= ~((1 << PJ0) | (1 << PJ1));
    DDRJ |= (1 << PJ0) | (1 << PJ1);
}

// =============================================================
// STANDARDOWY KOD ARDUINO
// =============================================================

const uint8_t relay_on[]  = {19, 17, 15, 23, 3, 5, 7, 9, 11, 13};
const uint8_t relay_dir[] = {18, 16, 14, 22, 2, 4, 6, 8, 10, 12};
const uint8_t PIN_COUNT = 10;

void setup() 
{
    Serial.begin(115200);
    
    // Druga warstwa zabezpieczenia - standardowe Arduino API
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        digitalWrite(relay_on[i], LOW);
        digitalWrite(relay_dir[i], LOW);
        pinMode(relay_on[i], OUTPUT);
        pinMode(relay_dir[i], OUTPUT);
    }
    
    Serial.println();
    Serial.println(F("================================="));
    Serial.println(F("   PIN TEST - BOOTLOADER DEBUG"));
    Serial.println(F("================================="));
    Serial.println(F("Jesli widzisz ten tekst, kod dziala."));
    Serial.println();
    Serial.println(F("Test 1: Sprawdz piny oscyloskopem podczas RESET"));
    Serial.println(F("Test 2: Obserwuj czy sa impulsy HIGH przed tym tekstem"));
    Serial.println();
    Serial.println(F("KOMENDY:"));
    Serial.println(F("  'h' - ustaw wszystkie piny HIGH"));
    Serial.println(F("  'l' - ustaw wszystkie piny LOW"));
    Serial.println(F("  't' - toggle test (miganie)"));
    Serial.println(F("  's' - pokaz stan pinow"));
    Serial.println(F("  'r' - symuluj RESET (soft reset)"));
    Serial.println();
}

void showPinStates()
{
    Serial.println(F("\n--- STAN PINOW ---"));
    Serial.println(F("RELAY_ON:"));
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        Serial.print(F("  Pin "));
        Serial.print(relay_on[i]);
        Serial.print(F(": "));
        Serial.println(digitalRead(relay_on[i]) ? "HIGH" : "LOW");
    }
    Serial.println(F("RELAY_DIR:"));
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        Serial.print(F("  Pin "));
        Serial.print(relay_dir[i]);
        Serial.print(F(": "));
        Serial.println(digitalRead(relay_dir[i]) ? "HIGH" : "LOW");
    }
}

void setAllPins(bool state)
{
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        digitalWrite(relay_on[i], state);
        digitalWrite(relay_dir[i], state);
    }
    Serial.print(F("Wszystkie piny: "));
    Serial.println(state ? "HIGH" : "LOW");
}

void toggleTest()
{
    Serial.println(F("Toggle test - 5 cykli..."));
    for (int c = 0; c < 5; c++) {
        setAllPins(HIGH);
        delay(200);
        setAllPins(LOW);
        delay(200);
    }
    Serial.println(F("Toggle test zakonczony."));
}

void softReset()
{
    Serial.println(F("Soft reset za 1 sekundę..."));
    delay(1000);
    // Watchdog reset
    asm volatile ("jmp 0");
}

void loop() 
{
    // LED heartbeat - pin 13 (jest w relay_on, więc używamy osobnego pinu)
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    if (millis() - lastBlink >= 500) {
        lastBlink = millis();
        ledState = !ledState;
        // Pin 13 jest w relay_on, więc miganie LED pokaże że kod działa
        // ale też będzie sygnałem dla kanału relay_on[9]
    }
    
    // Obsługa komend Serial
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'h':
            case 'H':
                setAllPins(HIGH);
                break;
            case 'l':
            case 'L':
                setAllPins(LOW);
                break;
            case 't':
            case 'T':
                toggleTest();
                break;
            case 's':
            case 'S':
                showPinStates();
                break;
            case 'r':
            case 'R':
                softReset();
                break;
        }
    }
}
