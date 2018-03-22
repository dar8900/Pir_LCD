#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

void LCDPrintString(int row, int col, String string);
void LCDPrintValue(int row, int col, int value);
void LCDPrintLineVoid(int row);
void LCDDisplayOn();
void ClearLCD();
void BlinkLed(int pin);

bool EnterSetupButton();
bool ChangeTimeBands();
bool InfoScroll();
bool SwichState();
bool ChangeValue();

#endif