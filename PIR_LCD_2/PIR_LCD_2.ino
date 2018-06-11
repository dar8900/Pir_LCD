#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp


#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"

#include "MenuFunc.h"
#include "LCDLib.h"
#include "TimeLib.h"
#include "Band.h"

FLAGS Flags;

EEPROM_ITEM EepromTab[] =
{
  {MIN_LIGHT_DELAY			,  START_DELAY_ADDR       , 1,  "Light delay"   , CHANGE_VALUE},
  {OFF				    	,  SWITCH_PIR_ADDR        , 1,  "PIR state"     , SWITCH_STATE},
  {OFF				      	,  MANUAL_STATE_ADDR      , 1,  "Manual state"  , SWITCH_STATE},
};



short CheckButtons()
{
	short ButtonPress = NOPRESS;
    short buttonUp = LOW, buttonDown = LOW, OkButton = LOW;
	buttonUp = READ(BUTTON_UP);
    buttonDown = READ(BUTTON_DOWN);
    OkButton = READ(BUTTON_SETUP);
	if(buttonUp == HIGH)
		ButtonPress = UP;
	if(buttonDown == HIGH)
		ButtonPress = DOWN;
	if(OkButton == HIGH)
		ButtonPress = OK_EXIT;
    if(buttonDown == HIGH && OkButton == HIGH)
        ButtonPress = EXIT_MANUAL;
	return ButtonPress;
}

// Blink del led per 5ms
void BlinkLed(short pin)
{
  digitalWrite(pin, HIGH);
  delay(5);
  digitalWrite(pin, LOW);
}

static void InitMemory()
{
	short numReg = 0;
	short FirstStartCheck = 0;

	ReadMemory(FIRST_START_CHECK_ADDR, 1, &FirstStartCheck);
	if(FirstStartCheck == 255)
	{
	  WriteMemory(FIRST_START_CHECK_ADDR, 1);
	  FirstStartCheck = 1;
	  LCDPrintString(0,CENTER_ALIGN, "Default values");
	  LCDPrintString(1, CENTER_ALIGN, "restored");
	  delay(2000);
	}
	else
	{
	  FirstStartCheck = 0;
	}

	// Leggi dalla memoria perchè non è il primo avvio
	if(FirstStartCheck == 0)
	{
		ReadMemory(NUM_REG_ADDR, 1, &numReg); // Inizializzo il numero registri per il delay
		ReadMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, numReg, &EepromTab[DELAY_AMOUNT].eeprom_par_value);
		ReadMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_numReg, &EepromTab[PIR_STATE].eeprom_par_value);
	}
	else // Imposta i valori di default
	{
		WriteMemory(NUM_REG_ADDR, 1);
		WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, EepromTab[DELAY_AMOUNT].eeprom_par_value);
		WriteMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_value);
	}
    EepromUpdate(LIGHT_STATE_ADDR, OFF);
	return;
}



void setup()
{

	Flags.Backlight = false;
	Flags.ManualState = false;

	Serial.begin(9600);

	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_SETUP, INPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(LIGHT_SWITCH, OUTPUT);
	pinMode(PIR_INPUT, INPUT);

	delay(1000);
    LCDInit();
	LCDNoBlink();
    RTCInit();

	InitMemory();
    BandInit();
	delay(1000);
	ClearLCD();
    OFF(LIGHT_SWITCH);
}

void loop()
{
    if(Flags.ManualState)
        ManualScreen();
    else
        MainScreen();
}
