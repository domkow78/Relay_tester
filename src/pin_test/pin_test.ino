/*
 * MINIMALNY TEST PINÓW - DIAGNOZA PROBLEMU BOOTLOADERA
 * =====================================================
 * 
 * WYNIKI DOTYCHCZASOWE:
 * - Pin 13 PULSUJE podczas hard reset -> BOOTLOADER używa go do LED!
 * - Pin 12 NIE pulsuje -> bootloader go nie dotyka
 * - Soft reset NIE wywołuje pulsowania -> omija bootloader
 * 
 * WNIOSEK: Bootloader Arduino Mega miga pin 13 (LED) podczas startu.
 * To normalne zachowanie, ale PROBLEM dla przekaźnika na tym pinie!
 * 
 * ROZWIĄZANIA dla pin 13:
 * 1. Przenieś przekaźnik z pin 13 na inny pin (ZALECANE)
 * 2. Wypal bootloader bez migania LED (Optiboot)
 * 3. Wgraj przez ISP bez bootloadera
 * 4. Dodaj RC delay na wejściu przekaźnika (hardware fix)
 * 
 * PINY RELAY_ON:  19, 17, 15, 23, 3, 5, 7, 9, 11, 30 
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
    
    // Port B: pin 11 (PB5), 10 (PB4), 12 (PB6)
    PORTB &= ~((1 << PB5) | (1 << PB4) | (1 << PB6));
    DDRB |= (1 << PB5) | (1 << PB4) | (1 << PB6);
    
    // Port C: pin 30 (PC7)
    PORTC &= ~(1 << PC7);
    DDRC |= (1 << PC7);
    
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

const uint8_t relay_on[]  = {19, 17, 15, 23, 3, 5, 7, 9, 11, 30};
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
    Serial.println(F("=========================================="));
    Serial.println(F("   PIN TEST - BOOTLOADER DEBUG v2"));
    Serial.println(F("=========================================="));
    Serial.println();
    Serial.println(F("!!! UWAGA: Pin 13 jest uzywany przez bootloader !!!"));
    Serial.println(F("    Bootloader miga LED (pin 13) podczas startu."));
    Serial.println(F("    To powoduje pulsowanie przekaznika na tym pinie!"));
    Serial.println();
    Serial.println(F("KOMENDY:"));
    Serial.println(F("  '1'-'9','0' - test pojedynczego pinu relay_on"));
    Serial.println(F("  'h' - wszystkie HIGH"));
    Serial.println(F("  'l' - wszystkie LOW"));
    Serial.println(F("  's' - pokaz stan pinow"));
    Serial.println(F("  'p' - test sekwencyjny (kazdy pin po kolei)"));
    Serial.println(F("  'r' - soft reset"));
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

void testSinglePin(uint8_t index)
{
    if (index >= PIN_COUNT) return;
    
    Serial.print(F("Test pin "));
    Serial.print(relay_on[index]);
    Serial.println(F(" - pulse HIGH/LOW 3x"));
    
    for (int i = 0; i < 3; i++) {
        digitalWrite(relay_on[index], HIGH);
        delay(300);
        digitalWrite(relay_on[index], LOW);
        delay(300);
    }
    Serial.println(F("Done."));
}

void sequentialTest()
{
    Serial.println(F("\n=== TEST SEKWENCYJNY RELAY_ON ==="));
    Serial.println(F("Kazdy pin HIGH na 1s, obserwuj oscyloskopem"));
    Serial.println();
    
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        Serial.print(F("Pin "));
        Serial.print(relay_on[i]);
        if (relay_on[i] == 13) Serial.print(F(" [BOOTLOADER LED!]"));
        Serial.println(F(" -> HIGH"));
        
        digitalWrite(relay_on[i], HIGH);
        delay(1000);
        digitalWrite(relay_on[i], LOW);
        delay(200);
    }
    
    Serial.println(F("\n=== TEST ZAKOŃCZONY ==="));
    Serial.println(F("Wszystkie piny LOW"));
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
            case 'p':
            case 'P':
                sequentialTest();
                break;
            case '1': testSinglePin(0); break;  // pin 19
            case '2': testSinglePin(1); break;  // pin 17
            case '3': testSinglePin(2); break;  // pin 15
            case '4': testSinglePin(3); break;  // pin 23
            case '5': testSinglePin(4); break;  // pin 3
            case '6': testSinglePin(5); break;  // pin 5
            case '7': testSinglePin(6); break;  // pin 7
            case '8': testSinglePin(7); break;  // pin 9
            case '9': testSinglePin(8); break;  // pin 11
            case '0': testSinglePin(9); break;  // pin 30
        }
    }
}
