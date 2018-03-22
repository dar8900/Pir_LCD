#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

void LCDPrintString(short row, short col, String string);
void LCDPrintValue(short row, short col, short value);
void LCDPrintLineVoid(short row);
void LCDDisplayOn();
void ClearLCD();
void BlinkLed(short pin);

bool EnterSetupButton();
bool ChangeTimeBands();
bool InfoScroll();
bool SwichState();
bool ChangeValue();

#endif