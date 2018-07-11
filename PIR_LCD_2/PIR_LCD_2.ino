#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp

#include "PIR_LCD_2.h"
#include "EEPROM_Ard.h"

#include "MenuFunc.h"
#include "LCDLib.h"
#include "TimeLib.h"
#include "Band.h"

#define KEYBOARD_DELAY	100

FLAGS Flags;

EEPROM_ITEM EepromTab[] =
{
  {MIN_LIGHT_DELAY			,  START_DELAY_ADDR       , 1,  "Light delay"   },
  {OFF				    	,  SWITCH_PIR_ADDR        , 1,  "PIR state"     },
  {OFF				      	,  MANUAL_STATE_ADDR      , 1,  "Manual state"  },
};



void ResetWD()
{
    wdt_reset();
}

// #define 	WDTO_15MS
// #define 	WDTO_30MS
// #define 	WDTO_60MS
// #define 	WDTO_120MS
// #define 	WDTO_250MS
// #define 	WDTO_500MS
// #define 	WDTO_1S
// #define 	WDTO_2S
// #define 	WDTO_4S
// #define 	WDTO_8S

void EnableWD(short Delay)
{
    wdt_enable(Delay);
}

short CheckButtons()
{
	short ButtonPress = NOPRESS;
    short buttonUp = LOW, buttonDown = LOW, OkButton = LOW;
	buttonUp = READ(BUTTON_UP);
    buttonDown = READ(BUTTON_DOWN);
    OkButton = READ(BUTTON_SETUP);
	if(buttonUp == HIGH)
    {
		ButtonPress = UP;
        delay(KEYBOARD_DELAY);
    }
	if(buttonDown == HIGH)
    {
		ButtonPress = DOWN;
        delay(KEYBOARD_DELAY);
    }
	if(OkButton == HIGH)
    {
		ButtonPress = OK_EXIT;
        delay(KEYBOARD_DELAY);
    }
    if(buttonDown == HIGH && OkButton == HIGH)
    {
        ButtonPress = EXIT_MANUAL;
		delay(KEYBOARD_DELAY);
    }
	
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
    short Reset = NO_RESET;

	ReadMemory(FIRST_START_CHECK_ADDR, 1, &FirstStartCheck);
    ReadMemory(RESET_DEFAULT_ADDR, 1, &Reset);
	if(FirstStartCheck == 255 || Reset == RESET)
	{
	  WriteMemory(FIRST_START_CHECK_ADDR, 1);
      WriteMemory(RESET_DEFAULT_ADDR, NO_RESET);
	  FirstStartCheck = 1;
	  LCDPrintString(ONE,CENTER_ALIGN, "Default values");
	  LCDPrintString(TWO, CENTER_ALIGN, "restored");
	  delay(1000);
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
        EepromUpdate(RESET_DEFAULT_ADDR, NO_RESET);
	}
	else // Imposta i valori di default
	{
		WriteMemory(NUM_REG_ADDR, 1);
		WriteMemory(EepromTab[DELAY_AMOUNT].eeprom_par_addr, EepromTab[DELAY_AMOUNT].eeprom_par_value);
		WriteMemory(EepromTab[PIR_STATE].eeprom_par_addr, EepromTab[PIR_STATE].eeprom_par_value);
		WriteMemory(LIGHT_STATE_ADDR, OFF);
        WriteMemory(BAND_INVALIDATION_VALUE_ADDR, INVALID);
		ChangeTime();
	}
    EepromUpdate(LIGHT_STATE_ADDR, OFF);
	return;
}

void LedCtrl(LED_POSITION Position)
{
    switch(Position)
    {
        case MENU_POS:
            ON(BLUE_LED);
            OFF(RED_LED);
            OFF(GREEN_LED);
            break;
        case PIR_INACTIVE_POS:
            OFF(BLUE_LED);
            ON(RED_LED);
            OFF(GREEN_LED);
            break;
        case PIR_ACTIVE_POS:
            OFF(BLUE_LED);
            OFF(RED_LED);
            ON(GREEN_LED);
            break;
        case MANUAL_POS:
            ON(BLUE_LED);
            OFF(RED_LED);
            ON(GREEN_LED);
            break;
        case MESSAGE_POS:
            ON(BLUE_LED);
            ON(RED_LED);
            OFF(GREEN_LED);
            break;
        case NO_LED:
            OFF(BLUE_LED);
            OFF(RED_LED);
            OFF(GREEN_LED);
            break;
		case ALL_LED:
            ON(BLUE_LED);
            ON(RED_LED);
            ON(GREEN_LED);
            break;
        default:
            break;
    }
}

void BlinkAllLed()
{
	LedCtrl(NO_LED);
	delay(5);
	LedCtrl(ALL_LED);
	delay(5);
}

void setup()
{
	Flags.ManualState = false;
    Flags.InBand = false;
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
    Flags.Backlight = true;
    LCDDisplayOn();
    //wdt_enable(WDTO_4S);
    //ResetWD();
}

void loop()
{
    if(Flags.ManualState)
        ManualScreen();
    else
        MainScreen();
}
