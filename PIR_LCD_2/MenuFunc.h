#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

void LCDPrintString(byte row, byte col, String string);
void LCDPrintValue(byte row, byte col, short value);
void LCDPrintLineVoid(byte row);
void LCDDisplayOn();
void ClearLCD();
void BlinkLed(byte pin);

bool EnterSetupButton();
bool ChangeTimeBands();
bool InfoScroll();
bool SwichState();
bool ChangeValue();

#endif