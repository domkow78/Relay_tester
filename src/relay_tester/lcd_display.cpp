#include "lcd_display.h"
#include "state_machine.h"
#include "eeprom_manager.h"
#include "config.h"
#include <LiquidCrystal_I2C.h>

#define LCD_UPDATE_INTERVAL 200  // ms - throttle odświeżania LCD

LiquidCrystal_I2C lcd(0x27,16,2);
static uint32_t lastLcdUpdate = 0;

void initLCD()
{
    lcd.init();
    lcd.backlight();
}

void updateLCD()
{
    // Throttle - odświeżaj LCD max co 200ms (I2C jest wolne)
    if(millis() - lastLcdUpdate < LCD_UPDATE_INTERVAL) return;
    lastLcdUpdate = millis();
    // LINIA 1: CYC:<licznik>
    lcd.setCursor(0,0);
    lcd.print("CYC:");
    lcd.print(state.cycle_counter);
    lcd.print("      ");  // wyczyść resztę

    // LINIA 2: [4 znaki stanu][spacja][10 znaków wzorca][spacja] = 16
    lcd.setCursor(0,1);

    const char* stateStr;
    switch(currentState)
    {
        case STATE_INIT:             stateStr = "INIT"; break;
        case STATE_IDLE:             stateStr = "IDLE"; break;
        case STATE_RUNNING:          stateStr = "RUN "; break;
        case STATE_STEP_ON:          stateStr = "ON  "; break;
        case STATE_STEP_OFF:         stateStr = "OFF "; break;
        case STATE_WAIT_DEAD_TIME:   stateStr = "DEAD"; break;
        case STATE_CHANGE_DIRECTION: stateStr = "DIR "; break;
        case STATE_WAIT_MEASUREMENT: stateStr = "MEAS"; break;
        case STATE_TEST_MODE:        stateStr = "TEST"; break;
        case STATE_FINISHED:         stateStr = "END "; break;
        case STATE_ERROR:            stateStr = "ERR "; break;
        default:                     stateStr = "?   "; break;
    }

    int8_t active = getActiveChannel();
    char line2[17];
    line2[0] = stateStr[0];
    line2[1] = stateStr[1];
    line2[2] = stateStr[2];
    line2[3] = stateStr[3];
    line2[4] = ' ';
    for(uint8_t i = 0; i < CHANNEL_COUNT; i++)
        line2[5 + i] = (i == (uint8_t)active) ? '1' : '0';
    line2[15] = ' ';
    line2[16] = '\0';

    lcd.print(line2);
}