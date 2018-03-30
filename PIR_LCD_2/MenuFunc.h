#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

enum
{
	CHANGE_HOUR = 0,
	CHANGE_MINUTE,
	CHANGE_DAY,
	CHANGE_MONTH,
	EXIT
};

enum
{
	CHANGE_BAND_1 = 0,
	CHANGE_BAND_2,
	CHEK_BAND,
	EXIT_CHANGE_BAND	
};

enum
{
	BAND_1 = 1,
	BAND_2
	
};



void LCDPrintString(short row, short col, String string);
void LCDPrintValue(short row, short col, short value);
void LCDPrintLineVoid(short row);
void LCDDisplayOn();
void ClearLCD();
void BlinkLed(short pin);
short ChekButtons();

bool EnterSetupButton();
bool ChangeTimeBands();
bool InfoScroll();
bool SwichState();
bool ChangeValue();

#endif