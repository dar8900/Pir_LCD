#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include <Arduino.h>

#define DELAY_MESSAGE_MENU  1200

enum
{
	ON = 0,
	OFF
};

void MainScreen(void);
void MainSetup(void);
void gestionePIR(short ActivePIR);
void ManualScreen(void);
bool ChangeTimeBands(void);
bool InfoScroll(void);
bool SwichState(void);
bool ChangeValue(void);

#endif
