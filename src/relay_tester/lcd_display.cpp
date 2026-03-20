#include "lcd_display.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include <LiquidCrystal_I2C.h>

#include "relay_control.h"
#include "state_machine.h"
#include <string.h>

LiquidCrystal_I2C lcd(0x27,16,2);

void initLCD()
{
    lcd.init();
    lcd.backlight();
}

const char* getStateString(SystemState s)
{
    switch(s)
    {
        case STATE_INIT: return "INIT";
        case STATE_IDLE: return "IDLE";
        case STATE_RUNNING: return "RUN ";
        case STATE_WAIT_MEASUREMENT: return "WAIT";
        case STATE_TEST_MODE: return "TEST";
        case STATE_FINISHED: return "END ";
        default: return "ERR ";
    }
}

void updateLCD()
{
    // LINIA 1
    lcd.setCursor(0,0);
    lcd.print("CYC:");
    lcd.print(state.cycle_counter);
    lcd.print("      ");

    // LINIA 2
    lcd.setCursor(0,1);

    // 1. STATE (lewa strona)
    const char* s = getStateString(currentState);
    lcd.print(s);

    // wyrównanie do 5 znaków
    int len = strlen(s);
    for(int i=len;i<5;i++)
        lcd.print(" ");

    // 2. spacja
    lcd.print(" ");

    // 3. wskaźniki przekaźników (10 szt)
    for(int i=0;i<CHANNEL_COUNT;i++)
    {
        lcd.print(relayState[i] ? "1":"0");
    }
}