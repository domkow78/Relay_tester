#include "lcd_display.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

void initLCD()
{
    lcd.begin(16,2);
    lcd.backlight();
}

void updateLCD()
{
    lcd.setCursor(0,0);
    lcd.print("CYC:");
    lcd.print(state.cycle_counter);

    lcd.setCursor(0,1);

    switch(currentState)
    {
        case STATE_RUNNING:
            lcd.print("RUN ");
            lcd.print("   ");
            break;

        case STATE_WAIT_MEASUREMENT:
            lcd.print("WAIT");
            lcd.print("   ");
            break;

        case STATE_IDLE:
            lcd.print("IDLE");
            lcd.print("   ");
            break;

        case STATE_FINISHED:
            lcd.print("END ");
            lcd.print("   ");
            break;

        default:
            lcd.print("    ");
    }
}