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
<<<<<<< HEAD
    lcd.print("      ");
=======
    lcd.print("    ");  // wyczyść resztę
>>>>>>> 368e6bd (improved structure and bug removing)

    // LINIA 2
    lcd.setCursor(0,1);

<<<<<<< HEAD
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
=======
    // linia 2: [4 znaki stanu][spacja][10 znaków wzorca][spacja] = 16
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
>>>>>>> 368e6bd (improved structure and bug removing)
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